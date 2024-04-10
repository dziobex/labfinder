#include <stdio.h>
#include "bits.h"

int main(int argc, int** argv) {
    int a;
    byte b;

    b = 127;
    a = 65535;

    for(int i = 0; i < BITSIZEOF(a); ++i) {
        printf("a is currently %d - bit #%d is %d\n", a, i, GETBIT(a, i));
        FLIPBIT(a, i);
        printf("a is now %d - bit %d flipped\n\n", a, i);
    }

    for(int j = 0; j < BITSIZEOF(b); ++j) {
        printf("b is currently %d - bit #%d is %d\n", b, j, GETBIT(b, j));
        SETBIT(b, j, 0);
        printf("b is now %d - bit %d set to 0\n\n", b, j);
    }
}