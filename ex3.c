#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "fixed_point.h"

int main(int argc, char **argv) {
    if (argc != 6) {
        printf("Usage: %s <x_raw> <a_raw> <b_raw> <c_raw> <q>\n", argv[0]);
        printf("All inputs must be integers. (x/a/b/c/q are int16 raw fixed-point values)\n");
        return 0;
    }

    // Read inputs as integers then cast to int16_t
    int x_in = atoi(argv[1]);
    int a_in = atoi(argv[2]);
    int b_in = atoi(argv[3]);
    int c_in = atoi(argv[4]);
    int q_in = atoi(argv[5]);

    int16_t x = (int16_t)x_in;
    int16_t a = (int16_t)a_in;
    int16_t b = (int16_t)b_in;
    int16_t c = (int16_t)c_in;
    int16_t q = (int16_t)q_in;

    eval_poly_ax2_minus_bx_plus_c_fixed(x, a, b, c, q);
    printf("\n");

    return 0;
}
