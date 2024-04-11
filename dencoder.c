#include "dencoder.h"

bit_pair get_bit_cords(short x) {
    bit_pair cord;
    cord.y = (x - x % 8) / 8;
    cord.x = x % 8;
    return cord;
}

byte encode_txt(FILE* input_file, byte maze_struct[][256],
    bit_pair* maze_size, maze_cord* in_cord, maze_cord* out_cord) {

    // for now, the assumptions are:
    // only type 0 (texted) is delivered, maze isn't damaged

    in_cord->x = in_cord->y = 1111;
    out_cord->x = out_cord->y = 1111;

    char c;
    short x = 0, y = 0;
    while ((c=getc(input_file)) != EOF) {
        if (c == 10) {      // going to the next line
            ++y;
            if (maze_size->x == 0)
                maze_size->x = x;
            else if (maze_size->x != x)
                return LINES_NOT_EQUAL;   // rows aren't equal, this maze is mazzed up 😭
            x = 0;
            continue;
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
            else if (x % 2 == 0 && y % 2 != 0) {     // WALLS info (for columns ||)
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
    maze_size->x = x / 2;
    maze_size->y = y / 2;

    if ( maze_size->y == 0 || maze_size->x == 0)
        return EMPTY_MAZE;
    if ( in_cord->x == 1111 || out_cord->x == 1111 )
        return NO_ENTRANCE;

    return 0;
}

void read_bits(byte bitter) {
    for ( size_t i = 7; i >= 0; --i) {
        printf("%d", (bitter >> i) & 0x01);
    }
}