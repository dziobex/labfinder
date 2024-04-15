#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "dencoder.h"
#include "bits.h"
#include "exit_codes.h"

void help(char* file) {
    printf("POTRZEBUJESZ POMOCY? ZADZWON: +48 800 70 2222\n\n");
    printf("UZYCIE PROGRAMU:\n\t%s -i <plik_wejsciowy> -o <plik_wyjsciowy> -c <kod_wejsciowy> -d <kod_wyjsciowy>\n", file);
}

int main(int argc, char **argv)
{
    byte maze_struct[1024 + 128][128 * 2] = {};
    byte maze_flags[1024][128] = {};

    char* in_file = NULL;
    char* out_file = NULL;
    char* in_code = NULL;
    char* out_code = NULL;

    int opt;
    while ((opt = getopt(argc, argv, ":i:o:c:d:")) != -1) {
        switch (opt) {
            case 'i':
                in_file = optarg;
                break;
            case 'o':
                out_file = optarg;
                break;
            case 'c':
                in_code = optarg;
                break;
            case 'd':
                out_code = optarg;
                break;
            default:
                return help(argv[0]), EXIT_FAILURE;
        }
    }

    if ( in_file == NULL || in_code == NULL )
        return help(argv[0]), EXIT_FAILURE;

    bit_pair maze_size;                         // size of the given maze
    maze_cord in_cord, out_cord;                // cords of the in/out points
    byte get_code = 1;                          // result of the file encoding

    FILE *in;                                   // file

    if ( strcmp(in_code, "t") == 0 ) {          // text coding
        in = fopen(in_file, "r");
        if (in == NULL)
            return fprintf(stderr, "Nie udalo sie otworzyc pliku wejsciowego :(\n"), EXIT_FAILURE;
        get_code = decode_txt(in, maze_struct, &maze_size, &in_cord, &out_cord);

    } else if ( strcmp(in_code, "b") == 0 ) {   // binary coding
        in = fopen(in_file, "rb");
        if (in == NULL)
            return fprintf(stderr, "Nie udalo sie otworzyc pliku wejsciowego :(\n"), EXIT_FAILURE;

            printf("AAAA");
        get_code = decode_binary(in, maze_struct, &maze_size, &in_cord, &out_cord);
    } else
        return fprintf(stderr, "Nieprawidlowy typ kodowania pliku wsadowego!\nSprobuj tych: tekstowy (t), binarny (b)\n"), EXIT_FAILURE;
    
    if ( strcmp(out_code, "t") != 0 && strcmp(out_code, "b") != 0 )
        return fprintf(stderr, "Nieprawidlowy typ kodowania pliku wyjsciowego!\nSprobuj tych: tekstowy (t), binarny (b)\n"), EXIT_FAILURE;

    fclose(in);

    switch (get_code) {
        default:
        case 0:
            printf("Labirynt zaakceptowany.\n");
            break;
        case LINES_NOT_EQUAL:
            printf("Nieprawidlowe rozmiary podanego labiryntu.\n");
            return EXIT_FAILURE;
        case MULTI_ENTRANCE:
            printf("Za duzo punktow wejscia/wyjscia.\n");
            return EXIT_FAILURE;
        case EMPTY_MAZE:
            printf("Pusty labirynt? Serio? Tylko na to cie stac?\n");
            return EXIT_FAILURE;
        case NO_ENTRANCE:
            printf("Brakuje punktu wejscia/wyjscia.\n");
            return EXIT_FAILURE;
    }
    
    free(in_file); free(out_file); free(in_code); free(out_code);

    return EXIT_SUCCESS;
}
