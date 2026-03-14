#include "../drivers/screen.h"
#include "sm_mem.h"

/* --- Hardware sin/cos via x87 FPU instructions --- */

#define TWO_PI  6.28318530717959f

static float k_sin(float x) {
    float result;
    __asm__ volatile("fsin" : "=t"(result) : "0"(x));
    return result;
}

static float k_cos(float x) {
    float result;
    __asm__ volatile("fcos" : "=t"(result) : "0"(x));
    return result;
}

/* Simple busy-wait delay between frames */
static void delay(void) {
    for (volatile int i = 0; i < 1500000; i++);
}

/* Initialize the x87 FPU (required on bare metal) */
static void fpu_init(void) {
    unsigned int cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 = (cr0 & ~(1 << 2)) | (1 << 1); /* clear EM, set MP */
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
    __asm__ volatile("fninit");
}

void entry(void) {
    fpu_init();

    float A = 0, B = 0;
    float z[1760];
    char b[1760];

    for (;;) {
        mem_set(b, ' ', 1760);
        mem_set((void *)z, 0, sizeof(z));

        float sinA = k_sin(A), cosA = k_cos(A);
        float sinB = k_sin(B), cosB = k_cos(B);

        float j, i;
        for (j = 0; j < TWO_PI; j += 0.07f) {
            float cosj = k_cos(j), sinj = k_sin(j);
            float cosj2 = cosj + 2;
            for (i = 0; i < TWO_PI; i += 0.02f) {
                float sini = k_sin(i),
                      cosi = k_cos(i),
                      mess = 1.0f / (sini * cosj2 * sinA + sinj * cosA + 5),
                      t = sini * cosj2 * cosA - sinj * sinA;
                int x = 40 + 30 * mess * (cosi * cosj2 * cosB - t * sinB);
                int y = 12 + 15 * mess * (cosi * cosj2 * sinB + t * cosB);
                int o = x + 80 * y;
                int N = 8 * ((sinj * sinA - sini * cosj * cosA) * cosB
                           - sini * cosj * sinA - sinj * cosA
                           - cosi * cosj * sinB);
                if (22 > y && y > 0 && x > 0 && 80 > x && mess > z[o]) {
                    z[o] = mess;
                    b[o] = ",.sbwlhdYEUD"[N > 0 ? N : 0];
                }
            }
        }

        /* Render using the printing library */
        blit_screen(b, 22, 80);

        A += 0.04f;
        B += 0.02f;
        delay();
    }
}
