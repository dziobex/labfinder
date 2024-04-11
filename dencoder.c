#include "dencoder.h"

bit_pair get_bit_cords(short x) {
    bit_pair cord;
    cord.y = (x - x % 8) / 8;
    cord.x = x % 8;
    return cord;
}

void encode(FILE* input_file, char coding_type, byte maze_struct[][256], bit_pair* maze_size) {
    // for now, the assumptions are:
    // only type 0 (texted) is delivered, maze isn't damaged

    char c;
    short x = 0, y = 0;
    while ((c=getc(input_file)) != EOF) {
        if (c == 10) {  // going to the next line
            ++y;
            if (maze_size->x == 0)
                maze_size->x = x / 2;
            x = 0;
            continue;
        }

        if (x != 0 && y != 0 ) {
            short cell_y = (y / 2) - 1;
            short cell_x = (x / 2) - 1;
            bit_pair _x = get_bit_cords(cell_x * 2);

            if (y % 2 == 0 && x % 2 != 0) {     // WALLS info (for rows =)
                cell_x++;
                _x = get_bit_cords(cell_x * 2);
                SETBIT(maze_struct[(int)cell_y][(int)_x.y], 7 - _x.x, c == 'X' ? 1 : 0);
            }
            else if (x % 2 == 0 && y % 2 != 0) {     // WALLS info (for columns ||)
                cell_y++;
                SETBIT(maze_struct[(int)cell_y][(int)_x.y], 7 - _x.x - 1, c == 'X' ? 1 : 0);
            }
        }

        ++x;
    }
    maze_size->y = (y) / 2;
}

void read_bits(byte bitter) {
    for ( int i = 7; i >= 0; --i) {
        printf("%d", (bitter >> i) & 0x01);
    }
}