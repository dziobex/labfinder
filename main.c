#include <stdio.h>
#include <stdlib.h>

#include "dencoder.h"
#include "bits.h"

int main( int argc, char** argv ) {

    byte maze_struct[1024][128 * 2] = {};
    byte maze_flags[1024][128] = {};

    printf("weight in total: %d KB\n\n", (sizeof(maze_struct) + sizeof(maze_flags))/ 1000);

    bit_pair maze_size;

    FILE* in = fopen("samples/01.txt", "r");
    encode(in, 0, maze_struct, &maze_size);

    printf("Dims of the maze: %d x %d\n", maze_size.y, maze_size.x);


    /*
    for ( int i = 0; i < maze_size.y; ++i ) {
        for ( int j = 0; j <= maze_size.x / 8; ++j ) {
            read_bits(maze_struct[i][j]);
            printf(" ");
        }
        printf("\n");
    }
    */

    return EXIT_SUCCESS;
}