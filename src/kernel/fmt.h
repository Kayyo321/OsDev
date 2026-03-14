#ifndef FmtH
#define FmtH

typedef struct {
    char *ptr;
} variadic_t;

variadic_t variadic_start(void *last_addr, unsigned int last_size);
void variadic_end(variadic_t *v);
void *variadic_arg(variadic_t *v, unsigned int size);

void fmt(char *fs, char *out_buf, unsigned int out_len, variadic_t variadic);
void fmt_s(char *fs, char *out_buf, unsigned int out_len, ...);

#endif