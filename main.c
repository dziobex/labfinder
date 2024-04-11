#include <stdio.h>
#include <stdlib.h>

#include "dencoder.h"
#include "bits.h"
#include "exit_codes.h"

int main( int argc, char** argv ) {

    byte maze_struct[1024][128 * 2] = {};
    byte maze_flags[1024][128] = {};

    bit_pair maze_size;
    maze_cord in_cord, out_cord;                      // punkty wejścia/wyjścia

    FILE* in = fopen("samples/00.txt", "r");

    printf("weight in total: %d KB\n\n", (sizeof(maze_struct)
        + sizeof(maze_flags)
        + sizeof(maze_size)
        + sizeof(in_cord)
        + sizeof(out_cord)
        + sizeof(in)) / 1000);

    byte get_code = encode_txt(in, maze_struct, &maze_size, &in_cord, &out_cord);
    
    fclose(in);

    switch (get_code) {
        default:
        case 0: {
            printf("Labirynt zaakceptowany.\n");
            break;
        }
        case LINES_NOT_EQUAL: {
            printf("Nieprawidlowe rozmiary podanego labiryntu.\n");
            return EXIT_FAILURE;
        }
        case MULTI_ENTRANCE: {
            printf("Za duzo punktow wejscia/wyjscia.\n");
            return EXIT_FAILURE;
        }
        case EMPTY_MAZE: {
            printf("Pusty labirynt? Tylko na to cie stac?\n");
            return EXIT_FAILURE;
        }
        case NO_ENTRANCE: {
            printf("Brakuje punktu wejscia/wyjscia.\n");
            return EXIT_FAILURE;
        }
    }

    printf("Wymiary labiryntu: %d x %d\n", maze_size.y, maze_size.x);

    return EXIT_SUCCESS;
}