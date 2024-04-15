#include "dencoder.h"

bit_pair get_bit_cords(short x) {
    bit_pair cord;
    cord.y = (x - x % 8) / 8;
    cord.x = x % 8;
    return cord;
}

byte decode_txt(FILE* input_file, byte maze_struct[][256],
    bit_pair* maze_size, maze_cord* in_cord, maze_cord* out_cord) {

    in_cord->x = in_cord->y = 1111;
    out_cord->x = out_cord->y = 1111;

    // JANOS JONOS TU BYŁ - "k00pa" <--- TO MÓJ CHŁOPAK!!!!!
    char c;
    short x = 0, y = 0;
    while ((c=getc(input_file)) != EOF) {
        if (c == 10) {                              // going to the next line
            if ( x == 0 )
                continue;                           // random empty lines in the file are ignored - mistakes happen!
            ++y;
            if (x != 0 && maze_size->x == 0)
                maze_size->x = x;
            else if (maze_size->x != x)
                return LINES_NOT_EQUAL;             // rows aren't equal, this maze is mazzed up 😭
            x = 0;
            continue;
        }
        
        if ( c != 'X' && c != ' ' && c != 'P' && c != 'K' && c != '\n' ) {
            printf("%c ", c);
        }
        if ((c == 'X' || c == ' ') && x != 0 && y != 0 ) {
            short cell_y = (y / 2) - 1;
            short cell_x = (x / 2) - 1;
            bit_pair _x = get_bit_cords(cell_x * 2);

            if (y % 2 == 0 && x % 2 != 0) {         // WALLS info (for rows =)
                cell_x++;
                _x = get_bit_cords(cell_x * 2);
                SETBIT(maze_struct[(int)cell_y][(int)_x.y], 7 - _x.x, c == 'X' ? 1 : 0);
            }
            else if (x % 2 == 0 && y % 2 != 0) {    // WALLS info (for columns ||)
                cell_y++;
                SETBIT(maze_struct[(int)cell_y][(int)_x.y], 7 - _x.x - 1, c == 'X' ? 1 : 0);
            }
        }
        if ( c == 'P' ) {
            if ( in_cord->x != 1111 )
                return MULTI_ENTRANCE;
            in_cord->y = y / 2;
            in_cord->x = x / 2;
        }
        if ( c == 'K' ) {
            if ( out_cord->x != 1111 )
                return MULTI_ENTRANCE;
            out_cord->y = y / 2;
            out_cord->x = x / 2;
        }
        ++x;
    }
    maze_size->x /= 2;
    maze_size->y = y / 2;

    // printf("%d %d ", maze_size->x, maze_size->y);

    if ( maze_size->y == 0 || maze_size->x == 0)
        return EMPTY_MAZE;
    if ( in_cord->x == 1111 || out_cord->x == 1111 )
        return NO_ENTRANCE;

    return 0;
}


byte decode_binary(FILE* input_file, byte maze_struct[][256], bit_pair* maze_size, maze_cord* in_cord, maze_cord* out_cord) {
    // 1. handling the HEADER section

    binary_data bd; // a container for (useless) data

    // problems with getting the file ID (structure thing)
    if ( fread(&bd.file_id, 4, 1, input_file) != 1 || fread(&bd.esc, 1, 1, input_file) != 1 )
        return INVALID_STRUCTURE;

    binary_pair dims, entry, exit;

    // problems with getting maze dims
    if ( fread(&dims.y, sizeof(uint16_t), 1, input_file) != 1 || fread(&dims.x, sizeof(uint16_t), 1, input_file) != 1 )
        return INVALID_STRUCTURE;
    maze_size->y = dims.y;
    maze_size->x = dims.x;

    // check the values of the dims
    if ( dims.x < 1 || dims.x > 2049  || dims.y < 1 || dims.y > 2049 )
        return INVALID_DIMS;

    // problems with getting the gates cords (entry)
    if ( fread(&entry.x, sizeof(uint16_t), 1, input_file) != 1 || fread(&entry.y, sizeof(uint16_t), 1, input_file) != 1 )
        return INVALID_STRUCTURE;

    // check the values of the entry cords
    if ( --entry.x < 0 || entry.x > 1023 || --entry.y < 0 || entry.y > 1023 )
        return INVALID_GATE;
    in_cord->x = entry.x;
    in_cord->y = entry.y;

    // problems with getting the gates cords (exit)
    if ( fread(&exit.x, sizeof(uint16_t), 1, input_file) != 1 || fread(&exit.y, sizeof(uint16_t), 1, input_file) != 1)
        return INVALID_STRUCTURE;

    // check the values of the exit cords
    if ( --exit.x < 0 || exit.x > 2048 || --exit.y < 0 || exit.y > 2048 )
        return INVALID_GATE;
    out_cord->x = entry.x;
    out_cord->y = entry.y;

    // getting the 'reserved' stuff
    if ( fread(&bd.reserved, sizeof(uint8_t), 12, input_file) != 12 )
        return INVALID_STRUCTURE;

    binary_data codes;
    // getting the counting stuff
    if ( fread(&codes.counter, sizeof(uint32_t), 1, input_file) != 1 || fread(&bd.solution_offset, sizeof(uint32_t), 1, input_file) != 1)
        return INVALID_STRUCTURE;

    // and here, getting the templates for coding characters
    uint8_t separator, wall, path;
    if ( fread(&separator, sizeof(uint8_t), 1, input_file) != 1 || fread(&wall, sizeof(uint8_t), 1, input_file) != 1 || fread(&path, sizeof(uint8_t), 1, input_file) != 1 )
        return INVALID_STRUCTURE;

    // 2. handling the CODING section

    uint8_t sep, val, count;
    bit_pair tracked_dims;
    tracked_dims.x = tracked_dims.y = 0;
    
    for ( ; codes.counter > 0; --codes.counter ) {
        if ( fread(&sep, sizeof(uint8_t), 1, input_file) != 1 || fread(&val, sizeof(uint8_t), 1, input_file) != 1 || fread(&count, sizeof(uint8_t), 1, input_file) != 1 )
            return LINES_NOT_EQUAL;

        if ( sep != separator || ( wall != val && path != val ) )
            return INVALID_CHARACTERS;

        // here will be coding the bit-maze

        for ( short i = 0; i <= count; ++i ) {
            if ( tracked_dims.x >= dims.x ) {
                tracked_dims.y++;
                tracked_dims.x = 0;
            }

            if (tracked_dims.x != 0 && tracked_dims.y != 0 ) {
                short cell_y = (tracked_dims.y / 2) - 1;
                short cell_x = (tracked_dims.x / 2) - 1;
                bit_pair _x = get_bit_cords(cell_x * 2);

                if (tracked_dims.y % 2 == 0 && tracked_dims.x % 2 != 0) {         // WALLS info (for rows =)
                    cell_x++;
                    _x = get_bit_cords(cell_x * 2);
                    SETBIT(maze_struct[(int)cell_y][(int)_x.y], 7 - _x.x, val == wall ? 1 : 0);
                }
                else if (tracked_dims.x % 2 == 0 && tracked_dims.y % 2 != 0) {     // WALLS info (for columns ||)
                    cell_y++;
                    SETBIT(maze_struct[(int)cell_y][(int)_x.y], 7 - _x.x - 1, val == wall ? 1 : 0);
                }
            }

            ++tracked_dims.x;
        }
    }

    maze_size->y /= 2;
    maze_size->x /= 2;

    return 0;   // valid maze

    // the 3. and 4. sections are ommitted during maze encoding
    // they can exist, but here we just don't care
}

void read_bits(byte bitter) {
    for ( char i = 7; i >= 0; --i) {
        printf("%d", (bitter >> i) & 0x01);
    }
}