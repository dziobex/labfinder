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
    // fprintf(output_file, "%" PRIu32, );

    // 07. solution offset
    fprintf(output_file, "%" PRIu32, (uint32_t)1);
    // 08. separator
    fprintf(output_file, "%" PRIu8, (uint8_t)'#');
    // 09. wall
    fprintf(output_file, "%" PRIu8, (uint8_t)'X');
    // 10. path
    fprintf(output_file, "%" PRIu8, (uint8_t)' ');

    // maze encoding


    // 12. file id (solution)
    fprintf(output_file, "%" PRIu32, 0x52524243);
    // 13. steps count in the solution
    char c;
    uint32_t steps_c = 0;
    while ((c = fgetc(steps_file)) != EOF)
        if (c == '\n')
            steps_c++;
    steps_c /= 2;
    steps_c--;                                  // STEPS are basically: direction, steps count, those are in 2 lines but are qualified as 1 step
    fprintf(output_file, "%" PRIu32, steps_c);
    fseek(steps_file, 0, SEEK_SET);             // now, taking steps vvvv

    // solution steps
    char dir = in_cord->dir;
    short dir_steps = 0;




    return 0;
}
