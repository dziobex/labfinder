#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "bits.h"

typedef struct {
    unsigned int y : 11;    // points the positon in the array of byted-objects (chars)
    unsigned int x : 11;    // points the position in the found object's bits
} bit_pair;

typedef struct {
    uint16_t x;
    uint16_t y;
} gate;

typedef union {
    uint32_t file_id;
    uint8_t esc;
    uint8_t reserved;
    uint32_t counter;
    uint32_t solution_offset;
} bin_data;

bit_pair get_bit_cords(short x);

byte encode_binary(FILE* input_file);