#pragma pack(1)

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
} binary_pair;

typedef union {
    uint32_t file_id;
    uint8_t esc;
    uint8_t reserved;   // 8*12=96
    uint32_t counter;
    uint32_t solution_offset;
} binary_data;

byte encode_binary(FILE* input_file, byte maze[][256]);