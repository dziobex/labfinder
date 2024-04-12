#include <stdio.h>
#include <stdlib.h>

#include "binny.h"
#include "bits.h"

int main (int argc, char **argv) {
    byte maze_struct[1024 + 128][128 * 2] = {};
    byte maze_flags[1024][128] = {};

    FILE* in = fopen("maze.bin", "rb");

    encode_binary(in, maze_struct);

    fclose(in);

    return 0;
}