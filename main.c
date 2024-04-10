#include <stdio.h>
#include <stdlib.h>

#include "dencoder.h"
#include "bits.h"

int main( int argc, char** argv ) {

    byte maze[1024][128 * 2] = {};

    printf("weight: %d KB\n\n", sizeof(maze) / 1000);

    bit_pair maze_size;

    FILE* in = fopen("samples/00.txt", "r");
    encode(in, 0, maze, &maze_size);

    printf("Dims of the maze's struct: %d x %d\n", maze_size.y, maze_size.x);

    return EXIT_SUCCESS;
}