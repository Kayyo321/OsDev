#ifndef ScreenH
#define ScreenH

enum {
    VideoAddress = 0xb8000,
    MaxRows = 25,
    MaxCols = 80,

    WhiteOnBlack = 0x0f,

    RegScrCtrl = 0x3D4,
    RegScrData = 0x3D5,
};

void print(char *fs, ...);
void println(char *fs, ...);
void print_raw(char *s);
void println_raw(char *s);
void clear_scr(void);
void reset_cursor(void);
void blit_screen(char *buf, int rows, int cols);

#endif