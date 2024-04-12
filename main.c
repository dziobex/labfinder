#include <stdio.h>
#include <stdlib.h>

#include "dencoder.h"
#include "bits.h"
#include "exit_codes.h"

int main( int argc, char** argv ) {

    byte maze_struct[1024 + 128][128 * 2] = {};
    byte maze_flags[1024][128] = {};

    // getopt HERE
    // ./a.out -in maze.txt -incode t -out solution.txt -outcode t

    bit_pair maze_size;
    maze_cord in_cord, out_cord;                      // punkty wejścia/wyjścia

    FILE* in = fopen("samples/00.txt", "r");

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

    return EXIT_SUCCESS;
}