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

    char *in_file = NULL, *out_file = NULL, *in_code = NULL, *out_code = NULL;
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
    byte get_code = 1;                          // result of the file de/en coding

    FILE *in;                                   // file
    
    if ( strcmp(in_code, "t") == 0 || strcmp(in_code, "b") == 0 ) {          // text coding
        in = fopen(in_file, (strcmp(in_code, "b") == 0) ? "rb" : "r" );
        if (in == NULL)
            return fprintf(stderr, "Nie udalo sie otworzyc pliku wejsciowego :(\n"), EXIT_FAILURE;
        get_code = strcmp(in_code, "b") == 0
            ? decode_binary(in, maze_struct, &maze_size, &in_cord, &out_cord)
            : decode_txt(in, maze_struct, &maze_size, &in_cord, &out_cord);
        fclose(in);
    } else
        return fprintf(stderr, "Nieprawidlowy typ kodowania pliku wsadowego!\nSprobuj tych: tekstowy (t), binarny (b)\n"), EXIT_FAILURE;

    if ( strcmp(out_code, "t") != 0 && strcmp(out_code, "b") != 0 )
        return fprintf(stderr, "Nieprawidlowy typ kodowania pliku wyjsciowego!\nSprobuj tych: tekstowy (t), binarny (b)\n"), EXIT_FAILURE;

    switch (get_code) {
        default:
        case 0:
            printf("Labirynt zaakceptowany.\n");
            break;
        case INVALID_DIMS:
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
        case INVALID_STRUCTURE:
            printf("Struktura kodowania podanego pliku jest nieprawidlowa.\n");
            return EXIT_FAILURE;
        case INVALID_GATE:
            printf("Nieprawidlowe koordynaty punktu wejscia/wyjscia.\n");
            return EXIT_FAILURE;
        case INVALID_CHARACTERS:
            printf("Uzyto nieprawidlowych znakow do kodowania labiryntu.\n");
            return EXIT_FAILURE;
    }

    // saving

    FILE *out;

    out = fopen(out_file, (strcmp(out_code, "b") == 0) ? "wb" : "w" );
    if (out == NULL)
        return fprintf(stderr, "Nie udalo sie otworzyc pliku wejsciowego :(\n"), EXIT_FAILURE;

    /*
    get_code = strcmp(out_code, "b") == 0
        ? encode_binary(out, maze_flags, &maze_size, &in_cord, &out_cord)
        : encode_txt(out, maze_flags, &maze_size, &in_cord, &out_cord);
    */
    get_code = 0;    // default value

    fclose(out);

    if ( get_code != 0 )
        return fprintf(stderr, "Cos poszlo nie tak z zapisem sciezki :(\n"), EXIT_FAILURE;
    printf("Sciezka zapisana do pliku %s :)\n", out_file);

    free(in_file); free(out_file); free(in_code); free(out_code);

    return EXIT_SUCCESS;
}