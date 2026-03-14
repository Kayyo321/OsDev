#include "sm_mem.h"

boolean_t mem_cmp(void *a, void *b, ul_t cnt) {
    unsigned char *pa = a;
    unsigned char *pb = b;
    while (cnt--) {
        if (*pa++ != *pb++)
            return False;
    }
    return True;
}

ul_t mem_mov(void *dest, void *src, ul_t cnt) {
    unsigned char *d = dest;
    unsigned char *s = src;
    if (d < s) {
        while (cnt--)
            *d++ = *s++;
    } else {
        d += cnt;
        s += cnt;
        while (cnt--)
            *--d = *--s;
    }
    return (ul_t)dest;
}

ul_t mem_set(void *dest, unsigned char b, ul_t cnt) {
    unsigned char *d = dest;
    while (cnt--)
        *d++ = b;
    return (ul_t)dest;
}
