#include "fmt.h"

#include "sm_mem.h"

unsigned long long __udivdi3(unsigned long long num, unsigned long long den) {
    unsigned long long quot = 0, qbit = 1;
    if (den == 0) return 0;
    while ((long long)den >= 0) {
        den <<= 1;
        qbit <<= 1;
    }
    while (qbit) {
        if (den <= num) {
            num -= den;
            quot += qbit;
        }
        den >>= 1;
        qbit >>= 1;
    }
    return quot;
}

unsigned long long __umoddi3(unsigned long long num, unsigned long long den) {
    return num - __udivdi3(num, den) * den;
}

static unsigned int variadic_align(unsigned int size) {
    return (size + sizeof(int) - 1) & ~(sizeof(int) - 1);
}

variadic_t variadic_start(void *last_addr, unsigned int last_size) {
    return (variadic_t) {
        .ptr = (char *)last_addr + variadic_align(last_size),
    };
}

void variadic_end(variadic_t *v) {
    v->ptr = (char *)0;
}

void *variadic_arg(variadic_t *v, unsigned int size) {
    void *arg = v->ptr;
    v->ptr += variadic_align(size);
    return arg;
}

static boolean_t is_eof(char ch) {
    return (ch == '\0');
}

static boolean_t insert_ch_to_buf(char ch, char *out_buf, ul_t *idx, unsigned int out_len) {
    if (*idx >= out_len)
        return True;

    out_buf[(*idx)++] = ch;
    return False;
}

static boolean_t insert_str_to_buf(char *s, char *out_buf, ul_t *idx, unsigned int out_len) {
    while (*s) {
        if (insert_ch_to_buf(*s++, out_buf, idx, out_len))
            return True;
    }
    return False;
}

static void unsigned_to_str(unsigned long long val, char *buf, int base, boolean_t uppercase) {
    char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    char tmp[65];
    int i = 0;

    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    while (val) {
        tmp[i++] = digits[val % base];
        val /= base;
    }

    int j = 0;
    while (i--)
        buf[j++] = tmp[i];
    buf[j] = '\0';
}

static void signed_to_str(long long val, char *buf, int base, boolean_t uppercase) {
    if (val < 0) {
        buf[0] = '-';
        unsigned_to_str((unsigned long long)(-val), buf + 1, base, uppercase);
    } else {
        unsigned_to_str((unsigned long long)val, buf, base, uppercase);
    }
}

static boolean_t handle_fmt_spec(char **fsp, char *out_buf, ul_t *idx, unsigned int out_len, variadic_t *v) {
    char *p = *fsp + 1;
    char spec[8];
    int si = 0;

    while (*p && *p != '}' && si < 7)
        spec[si++] = *p++;
    spec[si] = '\0';

    if (*p == '}')
        *fsp = p; 
    else
        *fsp = p - 1;

    int li = 0;
    int len_mod = 0; 
    if (spec[li] == 'l') {
        li++;
        len_mod = 1;
        if (spec[li] == 'l') {
            li++;
            len_mod = 2;
        }
    } else if (spec[li] == 'h') {
        li++;
        len_mod = -1;
        if (spec[li] == 'h') {
            li++;
            len_mod = -2;
        }
    }

    char conv = spec[li];
    char buf[65];

    switch (conv) {
        case 'd':
        case 'i': {
            long long val;
            if (len_mod == 2)
                val = *(long long *)variadic_arg(v, sizeof(long long));
            else if (len_mod == 1)
                val = *(long *)variadic_arg(v, sizeof(long));
            else
                val = *(int *)variadic_arg(v, sizeof(int));

            if (len_mod == -1)
                val = (short)val;
            else if (len_mod == -2)
                val = (signed char)val;

            signed_to_str(val, buf, 10, False);
            return insert_str_to_buf(buf, out_buf, idx, out_len);
        }

        case 'u': {
            unsigned long long val;
            if (len_mod == 2)
                val = *(unsigned long long *)variadic_arg(v, sizeof(unsigned long long));
            else if (len_mod == 1)
                val = *(unsigned long *)variadic_arg(v, sizeof(unsigned long));
            else
                val = *(unsigned int *)variadic_arg(v, sizeof(unsigned int));

            if (len_mod == -1)
                val = (unsigned short)val;
            else if (len_mod == -2)
                val = (unsigned char)val;

            unsigned_to_str(val, buf, 10, False);
            return insert_str_to_buf(buf, out_buf, idx, out_len);
        }

        case 'x':
        case 'X': {
            boolean_t upper = (conv == 'X');
            unsigned long long val;
            if (len_mod == 2)
                val = *(unsigned long long *)variadic_arg(v, sizeof(unsigned long long));
            else if (len_mod == 1)
                val = *(unsigned long *)variadic_arg(v, sizeof(unsigned long));
            else
                val = *(unsigned int *)variadic_arg(v, sizeof(unsigned int));

            if (len_mod == -1)
                val = (unsigned short)val;
            else if (len_mod == -2)
                val = (unsigned char)val;

            unsigned_to_str(val, buf, 16, upper);
            return insert_str_to_buf(buf, out_buf, idx, out_len);
        }

        case 'o': {
            unsigned long long val;
            if (len_mod == 2)
                val = *(unsigned long long *)variadic_arg(v, sizeof(unsigned long long));
            else if (len_mod == 1)
                val = *(unsigned long *)variadic_arg(v, sizeof(unsigned long));
            else
                val = *(unsigned int *)variadic_arg(v, sizeof(unsigned int));

            if (len_mod == -1)
                val = (unsigned short)val;
            else if (len_mod == -2)
                val = (unsigned char)val;

            unsigned_to_str(val, buf, 8, False);
            return insert_str_to_buf(buf, out_buf, idx, out_len);
        }

        case 'c': {
            char c = (char)*(int *)variadic_arg(v, sizeof(int));
            return insert_ch_to_buf(c, out_buf, idx, out_len);
        }

        case 's': {
            char *s = *(char **)variadic_arg(v, sizeof(char *));
            if (!s)
                s = "(null)";
            return insert_str_to_buf(s, out_buf, idx, out_len);
        }

        case 'p': {
            unsigned long long val = (unsigned long long)*(void **)variadic_arg(v, sizeof(void *));
            if (insert_str_to_buf("0x", out_buf, idx, out_len))
                return True;
            unsigned_to_str(val, buf, 16, False);
            return insert_str_to_buf(buf, out_buf, idx, out_len);
        }

        default:
            if (insert_ch_to_buf('{', out_buf, idx, out_len))
                return True;
            if (insert_str_to_buf(spec, out_buf, idx, out_len))
                return True;
            return insert_ch_to_buf('}', out_buf, idx, out_len);
    }
}

void fmt(char *fs, char *out_buf, unsigned int out_len, variadic_t variadic) {
    ul_t out_idx = 0;
    char ch;

    if (out_len > 0)
        --out_len;

    boolean_t leave = False;
    for (char *fsp = fs; fsp && !is_eof(*fsp) && !leave; ++fsp) {
        ch = *fsp;

        switch (ch) {
            case '\\':
                if (*(fsp+1) == '{') {
                    ++fsp; 
                    leave = insert_ch_to_buf('{', out_buf, &out_idx, out_len);
                } else {
                    leave = insert_ch_to_buf(ch, out_buf, &out_idx, out_len);
                }
                break;

            case '{':
                leave = handle_fmt_spec(&fsp, out_buf, &out_idx, out_len, &variadic);
                break;

            default:
                leave = insert_ch_to_buf(ch, out_buf, &out_idx, out_len);
                break;
        }
    }

    out_buf[out_idx] = '\0';
}

void fmt_s(char *fs, char *out_buf, unsigned int out_len, ...) {
    variadic_t v = variadic_start(&out_len, sizeof(out_len));
    fmt(fs, out_buf, out_len, v);

    variadic_end(&v);
}   
