#include "dencoder.h"

bit_pair get_bit_cords(short x) {
    bit_pair cord;
    cord.y = (x - x % 8) / 8;
    cord.x = x % 8;
    return cord;
}

void encode(FILE* input_file, char coding_type, byte maze[][256], bit_pair* maze_size) {
    // for now, the assumptions are:
    // only type 0 (texted) is delivered, maze isn't damaged

    char c;
    short x = 0, y = 0;
    while ((c=getc(input_file)) != EOF) {
        if (c == 10) {
            ++y;
            maze_size->x = x;
            x = 0;
        } else if (c == ' ' || c == 'X') {
            bit_pair cord = get_bit_cords(x);
            if (c == 'X')
                SETBIT(maze[y][cord.y], 7-cord.x, 1);
            else
                SETBIT(maze[y][cord.y], 7-cord.x, 0);
            ++x;
        }
    }
    maze_size->y = y + 1;
}