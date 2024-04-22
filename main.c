#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "bitish.h"
#include "dencoder.h"
#include "mazerunner.h"
#include "exit_codes.h"

void help(char* file);

int main(int argc, char **argv) {

    // our data containers
    byte maze_struct[1024][128 * 2] = {};
    byte maze_flags[1024][128] = {};

    // validating the args!
    
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

    // the most important info - from where we will get the maze and how is it coded
    // without it - farawell, miss/mr user!
    if ( in_file == NULL || in_code == NULL )
        return help(argv[0]), EXIT_FAILURE;

    bit_pair maze_size;
    maze_size.x = maze_size.y = 0;

    // coooords of in/out points
    maze_cord in_cord, out_cord;
    in_cord.x = in_cord.y = 1111;
    out_cord.x = out_cord.y = 1111;

    // get_code returns how the maze decoding went - failed or NOT
    byte get_code = 1;

    // from here we will (or not (︶︹︺) ), extract the maze
    FILE *in;
    
    // only if the coding types are T (text) or B (bext... binary I meant)
    
    if ( strcmp(in_code, "t") == 0 || strcmp(in_code, "b") == 0 ) {
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

    // how the maze decoding went - funny comments!
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
            printf("Pusty labirynt? Kurcze...\n");
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
    FILE *bfs_out = bfs_runner(maze_struct, maze_flags, maze_size, in_cord, out_cord);

    out = fopen(out_file, (strcmp(out_code, "b") == 0) ? "wb" : "w" );
    if (out == NULL)
        return fprintf(stderr, "Nie udalo sie otworzyc pliku wejsciowego :(\n"), EXIT_FAILURE;
    
    get_code = strcmp(out_code, "b") == 0
        ? encode_binary(out, bfs_out, maze_struct, &maze_size, &in_cord, &out_cord)
        : encode_txt(out, bfs_out);
    
    get_code = 0;    // default value

    fclose(out);

    if ( get_code != 0 )
        return fprintf(stderr, "Cos poszlo nie tak z zapisem sciezki :(\n"), EXIT_FAILURE;
    printf("Sciezka zapisana do pliku %s :)\n", out_file);

    return EXIT_SUCCESS;
}

void help(char* file) {
    printf("POTRZEBUJESZ POMOCY? ZADZWON: +48 800 70 2222\n\n");
    printf("UZYCIE PROGRAMU:\n\t%s -i <plik_wejsciowy> -o <plik_wyjsciowy> -c <kod_wejsciowy> -d <kod_wyjsciowy>\n", file);
}