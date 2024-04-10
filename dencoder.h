#include <stdio.h>
#include <stdlib.h>

#include "bits.h"

typedef struct {
    unsigned int y : 11;    // points the positon in the array of byted-objects (chars)
    unsigned int x : 11;    // points the position in the found object's bits
} bit_pair;

bit_pair get_bit_cords(short x);

void encode(FILE* input_file, char coding_type, byte maze[][256], bit_pair* maze_size);