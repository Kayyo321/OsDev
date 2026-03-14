#include "screen.h"

#include "../kernel/low_level.h"
#include "../kernel/fmt.h"
#include "../kernel/sm_mem.h"

static int get_screen_offset(int col, int row) {
    return (row * MaxCols + col) * 2;
}

static int get_cursor() {
    port_byte_out(RegScrCtrl, 14);
    int offset = port_byte_in(RegScrData) << 8;
    port_byte_out(RegScrCtrl, 15);
    offset += port_byte_in(RegScrData);
    return offset * 2;
}

static void set_cursor(int offset) {
    offset /= 2;
    port_byte_out(RegScrCtrl, 14);
    port_byte_out(RegScrData, (unsigned char)(offset >> 8));
    port_byte_out(RegScrCtrl, 15);
    port_byte_out(RegScrData, (unsigned char)(offset & 0xff));
}

static void handle_scrolling(int *offset) {
    if (*offset < MaxRows * MaxCols * 2) return;

    unsigned char *video_memory = (unsigned char*)VideoAddress;
    for (int i = 1; i < MaxRows; i++) {
        mem_mov(video_memory + get_screen_offset(0, i - 1),
                video_memory + get_screen_offset(0, i),
                MaxCols * 2);
    }

    char *last_line = (char*)(video_memory + get_screen_offset(0, MaxRows - 1));
    mem_set(last_line, 0, MaxCols * 2);

    *offset -= MaxCols * 2;
}

static void scribe_ch(char ch, int col, int row, char attr_b) {
    unsigned char *video_memory = (unsigned char*)VideoAddress;

    // if attribute byte is zero assume def style
    attr_b = (!attr_b) ? WhiteOnBlack : attr_b;

    int offset = (col >= 0 && row >= 0) ? get_screen_offset(col, row) : get_cursor();
    if (ch == '\n') { // if it's a newline, set the offset to the end of the current line so it advances to the next
        offset = get_screen_offset(MaxCols-1, offset / (MaxCols * 2));
    } else {
        video_memory[offset] = ch;
        video_memory[offset+1] = attr_b;
    }

    offset += 2;
    handle_scrolling(&offset);

    set_cursor(offset);
}

static void scribe_at(char *msg, int col, int row) {
    if (col >= 0 && row >= 0) {
        set_cursor(get_screen_offset(col, row));
    }

    for (int i = 0; msg[i] != '\0'; ++i) {
        scribe_ch(msg[i], col, row, WhiteOnBlack);
    }
}

void print(char *fs, ...) {
    char buf[128];
    mem_set(buf, '\0', 128);

    variadic_t v = variadic_start(&fs, sizeof(fs));
    fmt(fs, buf, 128, v);

    variadic_end(&v);

    scribe_at(buf, -1, -1);
}

void println(char *fs, ...) {
    char buf[128];
    mem_set(buf, '\0', 128);

    variadic_t v = variadic_start(&fs, sizeof(fs));
    fmt(fs, buf, 128, v);

    variadic_end(&v);

    scribe_at(buf, -1, -1);
    scribe_ch('\n', -1, -1, WhiteOnBlack);
}

void print_raw(char *s) {
    scribe_at(s, -1, -1);
}

void println_raw(char *s) {
    scribe_at(s, -1, -1);
    scribe_ch('\n', -1, -1, WhiteOnBlack);
}

void reset_cursor(void) {
    set_cursor(get_screen_offset(0, 0));
}

void blit_screen(char *buf, int rows, int cols) {
    unsigned char *video_memory = (unsigned char*)VideoAddress;
    for (int r = 0; r < rows; r++) {
        int src_off = r * cols;
        int dst_off = get_screen_offset(0, r);
        for (int c = 0; c < cols; c++) {
            video_memory[dst_off]     = buf[src_off + c];
            video_memory[dst_off + 1] = WhiteOnBlack;
            dst_off += 2;
        }
    }
}

void clear_scr(void) {
    for (int row = 0; row < MaxRows; ++row) {
        for (int col = 0; col < MaxCols; ++col) {
            scribe_ch(' ', col, row, WhiteOnBlack);
        }
    }

    set_cursor(get_screen_offset(0,0));
}
