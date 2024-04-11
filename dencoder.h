#include <stdio.h>
#include <stdlib.h>

#include "bits.h"
#include "exit_codes.h"

typedef struct {
    unsigned int y : 11;    // points the positon in the array of byted-objects (chars)
    unsigned int x : 11;    // points the position in the found object's bits
} bit_pair;

typedef struct {
    unsigned int y : 11;    // y-cord of the pointed cell
    unsigned int x : 11;    // x-cord of the pointed cell
    unsigned int dir : 2;   // where the entrance faces the cell:
                            // 0-TOP, 1-RIGHT, 2-BOTTOM, 3-LEFT
} maze_cord;

bit_pair get_bit_cords(short x);

byte encode_txt(FILE* input_file, byte maze_struct[][256],
    bit_pair* maze_size, maze_cord* in_cord, maze_cord* out_cord);

// binary representation of the given number
void read_bits(byte bitter);