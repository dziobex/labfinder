#include "dencoder.h"

// modules for file encoding

byte encode_txt( FILE* output_file, FILE* steps_file )
{
    fprintf(output_file, "START\n");
    char c;
    while ( (c = fgetc(steps_file)) != EOF )
        fprintf(output_file, "%c", c);
    fprintf(output_file, "STOP\n");
    return 0;
}

byte encode_binary(FILE* output_file, FILE* steps_file,
    byte maze_struct[][256], bit_pair* maze_size, maze_cord* in_cord, maze_cord* out_cord )
{
    // 01. FILE ID AND ESC SIGN:
    fprintf(output_file, "%" PRIu32 "%" PRIu8, 0x52524243, 0x1B);
    // 02. COLUMNS AND LINES (ROWS)
    fprintf(output_file, "%" PRIu16 "%" PRIu16, (uint16_t)(maze_size->y * 2 + 1), (uint16_t)(maze_size->x * 2 + 1));
    // 03. ENTRY
    fprintf(output_file, "%" PRIu16 "%" PRIu16, (uint16_t)(in_cord->x * 2 + 1), (uint16_t)(in_cord->y * 2 + 1));
    // 04. EXIT
    fprintf(output_file, "%" PRIu16 "%" PRIu16, (uint16_t)(out_cord->x * 2 + 1), (uint16_t)(out_cord->y * 2 + 1));
    // 05. RESERVED
    for ( char i = 0; i < 3; ++i )
        fprintf(output_file, "%" PRIu32, (uint32_t)0);

    // 06. counter
    FILE *tmp = tmpfile();
    uint32_t elements = 0;
    byte sign = 0, steps = 0;
        // first - the upper frame
    for( short i = 0; i < maze_size->x * 2 + 1; ++i) {
        if ( steps == 255 ) {   // max for byte
            ++i;                // steps-numeration from 0
            fprintf(tmp, "X %i\n", steps);
            steps = 0;
            ++elements;
        } else
            ++steps;
    }
    if ( steps > 0 ) {
        fprintf(tmp, "X %i\n", steps);      // without -1, beacuse the next line will start with X
        ++elements;
    }
        // middle - maze INSIDE analysis
    steps = 0;
    for( short y = 0; y < maze_size->y; ++y) {
        // looking at the 2rd bits of each cell (------ walls)
        for ( short x = 0; x < maze_size->x - 1; ++x ) {
            bit_pair bp = get_bit_cords(x * 2 + 1);
            if ( GETBIT((byte)maze_struct[bp.y], (int)bp.x) == 1 ) {
                fprintf(tmp, " %i\n", steps);
                fprintf(tmp, "X 0\n");
                elements += 2;
                steps = 0;
            } else
                ++steps;
            
            if ( steps == 255 ) {
                fprintf(tmp, " %i\n", steps);
                ++elements;
                steps = 0;
            }
        }
        fprintf(tmp, "X 1\n");
        ++elements;
        // looking at the 1st bits of each cell (||||||||| walls)
        for ( short x = 0; x < maze_size->x; ++x ) {
            bit_pair bp = get_bit_cords(x * 2);
            byte new_sign = GETBIT((byte)maze_struct[bp.y], (int)bp.x);

            if ( new_sign == sign ) {
                ++steps;
                if ( steps == 255 ) {
                    fprintf(tmp, "%c %i\n", sign == 0 ? ' ' : 'X', steps);
                    ++elements;
                    steps = 0;
                }
            } else {
                fprintf(tmp, "%c %i\n", sign == 0 ? ' ' : 'X', steps);
                ++elements;
                steps = 0;
                sign = new_sign;
            }
            if ( x == maze_size->x - 1) {
                fprintf(tmp, "%c %i\n", sign == 0 ? ' ' : 'X', steps);
                ++elements;
            }
        }
        fprintf(tmp, "X 1\n");
        ++elements;
    }
    steps = 0;
        // last - the lower frame
    for( short i = 0; i < maze_size->x * 2 + 1; ++i) {
        if ( steps == 255 ) {   // max for byte
            ++i;                // steps-numeration from 0
            fprintf(tmp, "X %i\n", steps);
            steps = 0;
            ++elements;
        } else
            ++steps;
    }
    if ( steps > 0 ) {
        fprintf(tmp, "X %i\n", steps);
        ++elements;
    }
    fprintf(output_file, "%" PRIu32, elements);
    // 07. solution offset
    fprintf(output_file, "%" PRIu32, (uint32_t)1);
    // 08. separator
    fprintf(output_file, "%" PRIu8, (uint8_t)'#');
    // 09. wall
    fprintf(output_file, "%" PRIu8, (uint8_t)'X');
    // 10. path
    fprintf(output_file, "%" PRIu8, (uint8_t)' ');
    // maze encoding
    rewind(tmp);
    char c = 0, buffer[20] = {};
    while (fgets(buffer, sizeof(buffer), tmp))
        if (sscanf(buffer, "%c %c", &c, &steps) == 2)
            fprintf(output_file, "#%c%c", &c, &steps);
    fclose(tmp);
    // 12. file id (solution)
    fprintf(output_file, "%" PRIu32, 0x52524243);
    // 13. steps count in the solution
    uint32_t steps_c = 0;
    while ((c = fgetc(steps_file)) != EOF)
        if (c == '\n')
            steps_c++;
    steps_c /= 2;
    if (steps_c > 0 ) steps_c--;                // STEPS are basically: direction, steps count, those are in 2 lines but are qualified as 1 step
    fprintf(output_file, "%" PRIu32, steps_c);
    // solution steps
    char dir = in_cord->dir;
    char buff[10] = {};
    uint32_t dir_steps = 0;
    rewind(steps_file);
    while (fgets(buffer, sizeof(buffer), steps_file)) {
        if (sscanf(buffer, "%s ", &buff) == 1) {
            if ( buff[0] == 'T') {
                sscanf(buffer, "%s ", &buff);
                if ( buff[0] == 'R')
                    dir++;
                else
                    dir--;
                if ( dir < 0 )
                    dir = 3;
                dir = dir % 4;
            } else {
                sscanf(buffer, "%s %d", &buff, &dir_steps);
                fprintf(output_file, "#%c%" PRIu32 "", dir == 0 ? 'N' : (dir == 1 ? 'E' : (dir == 2 ? 'S' : 'W')), dir_steps);
            }
        }
    }

    return 0;
}