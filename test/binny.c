#include "binny.h"

void read_bits(byte bitter) {
    for ( size_t i = 7; i >= 0; --i) {
        printf("%d", (bitter >> i) & 0x01);
    }
}

byte encode_binary(FILE* input_file) {

    // HEADER //

    bin_data bd; // useless data

    fread(&(bd.file_id), sizeof(uint32_t), 1, input_file);
    printf("ID pliku: 0x%X\n", bd.file_id);
    fread(&(bd.esc), sizeof(uint8_t), 1, input_file);

    uint16_t columns, rows;
    fread(&columns, sizeof(uint16_t), 1, input_file);
    fread(&rows, sizeof(uint16_t), 1, input_file);

    columns--;rows--;

    printf("Liczba kolumn: %d\nLiczba wierszy: %d\n", columns, rows);

    gate entry, exit;

    fread(&(entry.x), sizeof(uint16_t), 1, input_file);
    fread(&(entry.y), sizeof(uint16_t), 1, input_file);
    entry.x--;entry.y--;

    fread(&(exit.x), sizeof(uint16_t), 1, input_file);
    fread(&(exit.y), sizeof(uint16_t), 1, input_file);
    exit.x--;exit.y--;

    printf("Wejscie: (%d, %d)\nWyjscie: (%d, %d)\n", entry.x, entry.y, exit.x, exit.y);

    fread(&(bd.reserved), sizeof(uint8_t), 12, input_file);

    uint32_t codes_offset, solution_offset;
    fread(&(codes_offset), sizeof(uint32_t), 1, input_file);
    fread(&(solution_offset), sizeof(uint32_t), 1, input_file);

    uint8_t separator, wall, path;

    fread(&separator, sizeof(uint8_t), 1, input_file);
    fread(&wall, sizeof(uint8_t), 1, input_file);
    fread(&path, sizeof(uint8_t), 1, input_file);

    printf("Liczba slow kodowych: %d\nZnaczniki:\n\tseparator: %c\n\tsciana: %c\n\tsciezka: %c\n",
        codes_offset, separator, wall, path);

    uint8_t sep, val, count;
    codes_offset;

    uint16_t do_space = 0;

    // CODING WORDS //
    
    for ( ; codes_offset > 0; --codes_offset ) {
        fread(&sep, sizeof(uint8_t), 1, input_file);
        fread(&val, sizeof(uint8_t), 1, input_file);
        fread(&path, sizeof(uint8_t), 1, input_file);

        for ( size_t i = 0; i <= path; ++i )
            printf("%c", val);
    }
}