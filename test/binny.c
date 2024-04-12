#include "binny.h"

byte encode_binary(FILE* input_file, byte maze[][256]) {

    // 1. handling the HEADER section

    binary_data bd; // a container for (useless) data

    // problem z wczytywaniem id'ka
    if ( fread(&bd.file_id, 4, 1, input_file) != 1 )
        return 1;
    if ( fread(&bd.esc, 1, 1, input_file) != 1 )
        return 1;

    binary_pair dims, entry, exit;

    // problem with getting maze dims
    if ( fread(&dims.x, sizeof(uint16_t), 1, input_file) != 1 )
        return 1;
    if ( fread(&dims.y, sizeof(uint16_t), 1, input_file) != 1 )
        return 1;
    --dims.x;
    --dims.y;

    if ( fread(&entry.x, sizeof(uint16_t), 1, input_file) != 1 )
        return 1;
    if ( fread(&entry.y, sizeof(uint16_t), 1, input_file) != 1 )
        return 1;
    --entry.x;
    --entry.y;

    if ( fread(&exit.x, sizeof(uint16_t), 1, input_file) != 1 )
        return 1;
    if ( fread(&exit.y, sizeof(uint16_t), 1, input_file) != 1)
        return 1;
    --exit.x;
    --exit.y;

    // getting the 'reserved' stuff
    if ( fread(&bd.reserved, sizeof(uint8_t), 12, input_file) != 12 )
        return 1;

    binary_data codes;
    if ( fread(&codes.counter, sizeof(uint32_t), 1, input_file) != 1 )
        return 1;
    fread(&bd.solution_offset, sizeof(uint32_t), 1, input_file);    // no need for this for now

    uint8_t separator, wall, path;
    fread(&separator, sizeof(uint8_t), 1, input_file);
    fread(&wall, sizeof(uint8_t), 1, input_file);
    fread(&path, sizeof(uint8_t), 1, input_file);

    uint8_t sep, val, count;

    // 2. handling the CODING section

    bit_pair tracked_dims;
    tracked_dims.x = tracked_dims.y = 0;
    
    for ( ; codes.counter > 0; --codes.counter ) {
        fread(&sep, sizeof(uint8_t), 1, input_file);
        fread(&val, sizeof(uint8_t), 1, input_file);
        fread(&path, sizeof(uint8_t), 1, input_file);

        for ( size_t i = 0; i <= path; ++i ) {
            if ( tracked_dims.x >= dims.x ) {
                tracked_dims.y++;
                tracked_dims.x = 0;
            }
            printf("%c", val);
            ++tracked_dims.x;
        }
    }

    // the 3. and 4. sections are ommitted during maze encoding
    // they can exist, but here we just don't care

    /*
        when errors may occur:
        -user is stupid
        -dimensions are too small/big
        -counter (coding words) is smaller than (2xCOLS+1)*(2xROWS+1)
        -gates are out of range
        -missing section/section's part
        -user forgot what character it's signed for sep/wall/path XD
        !!!user is stupid!!!
    */

   /*
        -add to each 'fread' stuff: if case to check if the file IS NOT IN THE BERSERKER MODE HAIIIIYAYO 
   */
}