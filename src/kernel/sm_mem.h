#ifndef SmMemH
#define SmMemH

enum {
    True = 1,
    False = 0,
};

typedef unsigned long ul_t;
typedef unsigned char boolean_t;

boolean_t mem_cmp(void *a, void *b, ul_t cnt);
ul_t mem_mov(void *dest, void *src, ul_t cnt);
ul_t mem_set(void *dest, unsigned char b, ul_t cnt);

#endif