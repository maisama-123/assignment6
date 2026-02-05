#include "fixed_point.h"
#include <stdio.h>
#include <stdint.h>

void print_fixed(int16_t raw, int16_t q) {
    // Print raw / 2^q with exactly 6 decimal digits, truncated (not rounded)

    int32_t r = (int32_t)raw;
    int negative = (r < 0);

    int64_t absraw = negative ? -(int64_t)r : (int64_t)r;

    int64_t int_part;
    int64_t frac_part;

    if (q == 0) {
        int_part = absraw;
        frac_part = 0;
    } else {
        int64_t scale = 1LL << q;
        int_part = absraw / scale;
        int64_t frac_raw = absraw % scale;

        // frac_part = floor(frac_raw * 10^6 / 2^q)
        frac_part = (frac_raw * 1000000LL) / scale;
    }

    if (negative) printf("-");
    printf("%lld.%06lld", (long long)int_part, (long long)frac_part);
}

int16_t add_fixed(int16_t a, int16_t b) {
    return (int16_t)(a + b);
}

int16_t subtract_fixed(int16_t a, int16_t b) {
    return (int16_t)(a - b);
}

int16_t multiply_fixed(int16_t a, int16_t b, int16_t q) {
    // raw_out = (raw_a * raw_b) / 2^q  (truncate toward 0)
    int64_t prod = (int64_t)a * (int64_t)b;

    if (q == 0) {
        return (int16_t)prod;
    }

    int64_t denom = 1LL << q;
    int64_t res = prod / denom; // truncation toward 0
    return (int16_t)res;
}

void eval_poly_ax2_minus_bx_plus_c_fixed(int16_t x, int16_t a, int16_t b, int16_t c, int16_t q) {
    // y = a*x^2 - b*x + c

    int16_t x2 = multiply_fixed(x, x, q);        // x^2 in Qq
    int16_t ax2 = multiply_fixed(a, x2, q);      // a*x^2
    int16_t bx  = multiply_fixed(b, x, q);       // b*x

    int16_t y = subtract_fixed(ax2, bx);         // a*x^2 - b*x
    y = add_fixed(y, c);                         // + c

    printf("the polynomial output for a=");
    print_fixed(a, q);
    printf(", b=");
    print_fixed(b, q);
    printf(", c=");
    print_fixed(c, q);
    printf(" is ");
    print_fixed(y, q);
}
