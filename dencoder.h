#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "bits.h"
#include "exit_codes.h"

typedef struct {
    unsigned int y : 11;    // points the positon in the array of byted-objects (chars)
    unsigned int x : 11;    // points the position in the found object's bits
} bit_pair;

typedef struct {
    uint16_t x;
    uint16_t y;
} binary_pair;

typedef struct {
    unsigned int y : 11;    // y-cord of the pointed cell
    unsigned int x : 11;    // x-cord of the pointed cell
    unsigned int dir : 2;   // where the entrance faces the cell:
                            // 0-TOP, 1-RIGHT, 2-BOTTOM, 3-LEFT
} maze_cord;

typedef union {
    uint32_t file_id;
    uint8_t esc;
    uint8_t reserved;       // 8 * 12 = 96
    uint32_t counter;
    uint32_t solution_offset;
} binary_data;

// get the REAL cords in the bit-matrix
bit_pair get_bit_cords(short x);

// binary representation of the given number
void read_bits(byte bitter);

// RECEIVING

byte decode_txt(FILE* input_file, byte maze_struct[][256],
    bit_pair* maze_size, maze_cord* in_cord, maze_cord* out_cord);
byte decode_binary(FILE* input_file, byte maze_struct[][256],
    bit_pair* maze_size, maze_cord* in_cord, maze_cord* out_cord);

// GIVING

byte encode_txt(FILE* output_file, FILE* steps_file );
byte encode_binary(FILE* output_file, FILE* steps_file,
    byte maze_struct[][256], bit_pair* maze_size, maze_cord* in_cord, maze_cord* out_cord );
