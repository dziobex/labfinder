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
    uint32_t file_id = 0x52524243;
    uint8_t esc_sign = 0x1B;
    fwrite(&file_id, sizeof(uint32_t), 1, output_file);
    fwrite(&esc_sign, sizeof(uint8_t), 1, output_file);
    
    // 02. COLUMNS AND LINES (ROWS)
    uint16_t columns = (uint16_t)(maze_size->y * 2 + 1);
    uint16_t rows = (uint16_t)(maze_size->x * 2 + 1);
    fwrite(&columns, sizeof(uint16_t), 1, output_file);
    fwrite(&rows, sizeof(uint16_t), 1, output_file);
    
    // 03. ENTRY
    uint16_t entry_x = (uint16_t)(in_cord->x * 2 + 1);
    uint16_t entry_y = (uint16_t)(in_cord->y * 2 + 1);
    fwrite(&entry_x, sizeof(uint16_t), 1, output_file);
    fwrite(&entry_y, sizeof(uint16_t), 1, output_file);
    
    // 04. EXIT
    uint16_t exit_x = (uint16_t)(out_cord->x * 2 + 1);
    uint16_t exit_y = (uint16_t)(out_cord->y * 2 + 1);
    fwrite(&exit_x, sizeof(uint16_t), 1, output_file);
    fwrite(&exit_y, sizeof(uint16_t), 1, output_file);
    
    // 05. RESERVED
    uint32_t reserved = 0;
    fwrite(&reserved, sizeof(uint32_t), 3, output_file);

    // 06. counter
    FILE *tmp = tmpfile();
    uint32_t elements = 0;
    byte sign = 0, steps = 0;
    
    uint8_t separator = '#';
    uint8_t wall = 'X';
    uint8_t path = ' ';

    // the upper frame
    for( short i = 0; i < maze_size->x * 2 + 1; ++i) {
        if ( steps == 255 ) {   // max for byte
            ++i;                // steps-numeration from 0
            fwrite(&sign, sizeof(byte), 1, tmp);
            fwrite(&steps, sizeof(byte), 1, tmp);
            steps = 0;
            ++elements;
        } else
            ++steps;
    }
    if ( steps > 0 ) {
        fwrite(&sign, sizeof(byte), 1, tmp);
        fwrite(&steps, sizeof(byte), 1, tmp);
        ++elements;
    }

    // maze INSIDE analysis
    steps = 0;
    for( short y = 0; y < maze_size->y; ++y) {
        // looking at the 2nd bits of each cell (------ walls)
        for ( short x = 0; x < maze_size->x - 1; ++x ) {
            bit_pair bp = get_bit_cords(x * 2 + 1);
            if (GETBIT((byte)maze_struct[bp.y], (int)bp.x) == 1) {
                fwrite(&sign, sizeof(byte), 1, tmp);
                fwrite(&steps, sizeof(byte), 1, tmp);
                steps = 0;
                ++elements;
            } else
                ++steps;
            
            if ( steps == 255 ) {
                fwrite(&sign, sizeof(byte), 1, tmp);
                fwrite(&steps, sizeof(byte), 1, tmp);
                ++elements;
                steps = 0;
            }
        }
        sign = 0;
        steps = 1;
        fwrite(&sign, sizeof(byte), 1, tmp);
        fwrite(&steps, sizeof(byte), 1, tmp);
        ++elements;

        // looking at the 1st bits of each cell (||||||||| walls)
        for ( short x = 0; x < maze_size->x; ++x ) {
            bit_pair bp = get_bit_cords(x * 2);
            byte new_sign = GETBIT((byte)maze_struct[bp.y], (int)bp.x);

            if ( new_sign == sign ) {
                ++steps;
                if ( steps == 255 ) {
                    fwrite(&sign, sizeof(byte), 1, tmp);
                    fwrite(&steps, sizeof(byte), 1, tmp);
                    ++elements;
                    steps = 0;
                }
            } else {
                fwrite(&sign, sizeof(byte), 1, tmp);
                fwrite(&steps, sizeof(byte), 1, tmp);
                ++elements;
                steps = 0;
                sign = new_sign;
            }
            if ( x == maze_size->x - 1) {
                fwrite(&sign, sizeof(byte), 1, tmp);
                fwrite(&steps, sizeof(byte), 1, tmp);
                ++elements;
            }
        }
        sign = 0;
        steps = 1;
        fwrite(&sign, sizeof(byte), 1, tmp);
        fwrite(&steps, sizeof(byte), 1, tmp);
        ++elements;
    }

    // the lower frame
    steps = 0;
    for( short i = 0; i < maze_size->x * 2 + 1; ++i) {
        if ( steps == 255 ) {   // max for byte
            ++i;                // steps-numeration from 0
            fwrite(&sign, sizeof(byte), 1, tmp);
            fwrite(&steps, sizeof(byte), 1, tmp);
            steps = 0;
            ++elements;
        } else
            ++steps;
    }
    if ( steps > 0 ) {
        fwrite(&sign, sizeof(byte), 1, tmp);
        fwrite(&steps, sizeof(byte), 1, tmp);
        ++elements;
    }

    fwrite(&elements, sizeof(uint32_t), 1, output_file);

    // 07. solution offset
    uint32_t solution_offset = 1;
    fwrite(&solution_offset, sizeof(uint32_t), 1, output_file);
    
    // 08. sep
    fwrite(&separator, sizeof(uint8_t), 1, output_file);
    
    // 09. wall
    fwrite(&wall, sizeof(uint8_t), 1, output_file);
    
    // 10. path
    fwrite(&path, sizeof(uint8_t), 1, output_file);

    // maze ENCOIDNG
    rewind(tmp);
    byte c;
    while (fread(&c, sizeof(byte), 1, tmp)) {
        fwrite(&separator, sizeof(uint8_t), 1, output_file);
        fwrite(&c, sizeof(byte), 1, output_file);
    }
    fclose(tmp);

    // 12. file ID (solution)
    fwrite(&file_id, sizeof(uint32_t), 1, output_file);
    
    // 13. steps count in the solution
    uint32_t steps_c = 0;
    while (fgetc(steps_file) != EOF)
        steps_c++;
    steps_c /= 2;
    if (steps_c > 0) steps_c--;
    fwrite(&steps_c, sizeof(uint32_t), 1, output_file);
    
    // solution encoiding
    fseek(steps_file, 0, SEEK_SET);
    byte dir;
    uint32_t dir_steps;
    while (fscanf(steps_file, "%c %" PRIu32 "", &dir, &dir_steps) == 2) {
        fwrite(&separator, sizeof(uint8_t), 1, output_file);
        fwrite(&dir, sizeof(byte), 1, output_file);
        fwrite(&dir_steps, sizeof(uint32_t), 1, output_file);
    }
    
    return 0;
}