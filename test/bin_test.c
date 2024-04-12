#include <stdio.h>
#include <stdlib.h>

#include "binny.h"

int main (int argc, char **argv) {

    FILE* in = fopen("maze.bin", "rb");

    encode_binary(in);

    fclose(in);

    return 0;
}