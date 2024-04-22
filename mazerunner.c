#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitish.h"
#include "dencoder.h"

//So, maze runner.
// BFS with its queue stored in a tempfile
// easy to say, but we'll also need to be able to swap between 2 temporary maze files
// Mmm. Okay.

/*
Each cell in the maze is stored in maze_build and maze_nodes
nodes says if i visited it,
maze_build uses 2 bits - can i go down, can i go right

this means starting at a given pos i'll have to check...
hm, i'll have to check itself and the 2 members < and ^
but that's doable given we're moving in 2-bit intervals

so we have everything in memory. just up to decoding it
*/

byte unfound(maze_cord cur, maze_cord want) {
    if (cur.x == want.x - 1 && cur.y == want.y) return 0;
    else if (cur.x == want.x && cur.y == want.y - 1) return 0;
    //Wygląda na to, że labirynty \ i - mają pierwszy przypadek, a labirynty | drugi.
    //Why fix it when you can make a workaround? lol
    return 1;
} //make this a ternary perhaps

maze_cord make_coord(unsigned short x, unsigned short y, byte dir) {
    maze_cord construct;
    construct.x = x;
    construct.y = y;
    construct.dir = dir;

    return construct;
}

byte valid_coords(maze_cord checked_pos, bit_pair maze_size) {
    if (checked_pos.x < 0 || checked_pos.x >= maze_size.x) return 0;
    else if (checked_pos.y < 0 || checked_pos.y >= maze_size.y) return 0;
    return 1;
}

byte construct_info_byte(byte maze_build[][256], byte maze_nodes[][128], maze_cord maze_pos, bit_pair maze_size) {
    byte info_byte = 0xFF;
    /*
     * Info Byte construction:
     * First 4 bytes go v>^<, describes if there's a path (0) or wall (1) adjacent
     * Last 4 bytes also go v>^<, describes if has been explored or not
     * outside of labirynth means it's an explored wall as well.
    */
    SETBIT(maze_nodes[maze_pos.y][maze_pos.x / 8], 7 - (maze_pos.x) % 8, 1); //This is important LOL
    //printf("\nEntered construct, pos is %d %d\n", maze_pos.x, maze_pos.y);
    //While a loop might be possible with an eval, too lazy lol

    //Checks down
    if(valid_coords(make_coord(maze_pos.x, maze_pos.y + 1, maze_pos.dir), maze_size)) {
        /*printf("Down is a valid coordinate!\n");
        printf("Looking for byte %d of %d %d (", (7 - (maze_pos.x) % 8), maze_pos.y+1, maze_pos.x / 8);
        for (short dds = 7; dds >= 0; --dds) {
            printf("%d", GETBIT(maze_nodes[maze_pos.y+1][maze_pos.x / 8], dds));
        }
        printf(")\n");*/
        SETBIT(info_byte, 0, GETBIT(maze_build[maze_pos.y][maze_pos.x / 4], 7 - (2 *((maze_pos.x) % 4) )));
        
        SETBIT(info_byte, 4, GETBIT(maze_nodes[maze_pos.y+1][maze_pos.x / 8], 7 - (maze_pos.x) % 8));
    }
    //Then to the right
    if(valid_coords(make_coord(maze_pos.x + 1, maze_pos.y, maze_pos.dir), maze_size)) {
        SETBIT(info_byte, 1, GETBIT(maze_build[maze_pos.y][maze_pos.x / 4], 7 - (2 *((maze_pos.x) % 4) + 1)));

        SETBIT(info_byte, 5, GETBIT(maze_nodes[maze_pos.y][(maze_pos.x + 1) / 8], 7 - (maze_pos.x + 1) % 8));
    }
    //Then up
    if(valid_coords(make_coord(maze_pos.x, maze_pos.y - 1, maze_pos.dir), maze_size)) {
        SETBIT(info_byte, 2, GETBIT(maze_build[maze_pos.y - 1][maze_pos.x / 4], 7 -  (2 *((maze_pos.x) % 4))));

        SETBIT(info_byte, 6, GETBIT(maze_nodes[maze_pos.y-1][maze_pos.x / 8], 7 - (maze_pos.x) % 8));
    }
    //Then to the left
    if(valid_coords(make_coord(maze_pos.x - 1, maze_pos.y, maze_pos.dir), maze_size)) {
        SETBIT(info_byte, 3, GETBIT(maze_build[maze_pos.y][(maze_pos.x - 1) / 4], 7 - (2 *((maze_pos.x - 1) % 4) + 1)));

        SETBIT(info_byte, 7, GETBIT(maze_nodes[maze_pos.y][(maze_pos.x - 1) / 8], 7 - (maze_pos.x - 1) % 8));
    }

    /*for (byte dsds = 0; dsds < 8; ++dsds) {
        printf("%d", GETBIT(info_byte, dsds));
    }
    printf("\n");*/
    return info_byte;
}

void enqueue(FILE* queue, byte maze_nodes[][128], maze_cord maze_pos, byte info_byte, long long queue_parent_pos, char* buffer) {
    long long line_start;
    sprintf(buffer, " "); //Purge excess buffer data (IMPORTANT!)
    for(byte i = 0; i < 4; ++i) {
        if(GETBIT(info_byte, i) == 0 && GETBIT(info_byte, i+4) == 0) {
            line_start = ftell(queue);
            switch(i) {
                case 0:
                    snprintf(buffer, 160, "%lld:%d %d 0:%lld\n", queue_parent_pos, maze_pos.x, maze_pos.y + 1, line_start);
                    fwrite(buffer, 1, strlen(buffer), queue);
                    break;
                case 1:
                    snprintf(buffer, 160, "%lld:%d %d 3:%lld\n", queue_parent_pos, maze_pos.x + 1, maze_pos.y, line_start);
                    fwrite(buffer, 1, strlen(buffer), queue);
                    break;
                case 2:
                    snprintf(buffer, 160, "%lld:%d %d 2:%lld\n", queue_parent_pos, maze_pos.x, maze_pos.y - 1, line_start);
                    fwrite(buffer, 1, strlen(buffer), queue);
                    break;
                case 3:
                    snprintf(buffer, 160, "%lld:%d %d 1:%lld\n", queue_parent_pos, maze_pos.x - 1, maze_pos.y, line_start);
                    fwrite(buffer, 1, strlen(buffer), queue);
                    break;
            }
        }
    }
}

maze_cord dequeue(FILE* queue, long long* queue_parent_pos, long long queue_bin_pos, char* buffer) {
    //printf("at %ld right before fseek \n", ftell(queue));
    fseek(queue, queue_bin_pos, SEEK_SET);
    //printf("at %ld right after fseek \n", ftell(queue)); // There we go. So this says it doesn't reseek properly.
    fgets(buffer, 160, queue); 
    //printf("at %ld while dequeueing \n", ftell(queue));

    maze_cord return_value = make_coord(0, 0, 0x00);

    //Yep, I'm making my own atoi. Why?
    //Several 'ints' i need to split. I don't feel like thinking about the headache too much.
    *queue_parent_pos = 0;

    byte divOccurs = 0;
    byte grabbingPos = 0;
    byte counter = 0;
    while (buffer[counter] != '\0') {
        switch(buffer[counter]) {
            case '\n':
                break;
            case ':':
                grabbingPos = 1 - grabbingPos;
                break;
            case ' ':
                ++divOccurs;
                break;
            default:
                if (grabbingPos) {
                    switch (divOccurs) {
                        case 0:
                            return_value.x *= 10;
                            return_value.x += (buffer[counter] - '0');
                            break;
                        case 1:
                            return_value.y *= 10;
                            return_value.y += (buffer[counter] - '0');
                            break;
                        case 2:
                            return_value.dir = (buffer[counter] - '0'); //Guaranteed to be one digit.
                            break;
                    }
                }
                else {
                    if (buffer[counter] == '-') {
                        ++counter;
                        break;
                    } else {
                        if (divOccurs == 0) {
                            break;
                        } else {
                            *queue_parent_pos *= 10;
                            *queue_parent_pos += (buffer[counter] - '0');
                        }
                    }
                }
                break;
        }
        ++counter;
    }

    return return_value;
}

FILE* reconstruct_path(FILE* queue, maze_cord in_cord, maze_cord out_cord, long long queue_bin_pos, char* buffer) {
    FILE* inverted_out = tmpfile();
    FILE* unconverted_out = tmpfile();
    FILE* actual_out = tmpfile();

    maze_cord temp_coord = make_coord(0, 0, 0x00);
    byte divOccurs = 0;
    byte grabbingPos = 0;
    byte counter = 0;
    long long temp_pos = -2;

    fseek(queue, queue_bin_pos, SEEK_SET);

    fputc('0' + out_cord.dir, inverted_out);
    fputc('-', inverted_out);
    while(temp_pos != -1) {
        fgets(buffer, 160, queue);
        //fputs(buffer, stdout);
        divOccurs = 0;
        counter = 0;

        temp_pos = 0;
        temp_coord.x = 0;
        temp_coord.y = 0;
        while (buffer[counter] != '\0') {
            switch(buffer[counter]) {
                case ':':
                    grabbingPos = 1 - grabbingPos;
                    break;
                case ' ':
                    ++divOccurs;
                    break;
                default:
                    if (grabbingPos) {
                        switch (divOccurs) {
                            case 0:
                                temp_coord.x *= 10;
                                temp_coord.x += (buffer[counter] - '0');
                                break;
                            case 1:
                                temp_coord.y *= 10;
                                temp_coord.y += (buffer[counter] - '0');
                                break;
                            case 2:
                                temp_coord.dir = (buffer[counter] - '0'); //Guaranteed to be one digit.
                                break;
                        }
                    }
                    else {
                        if (buffer[counter] == '-') {
                            temp_pos = -1;
                            break;
                        } else {
                            if (divOccurs == 0 && temp_pos != -1) {
                                temp_pos *= 10;
                                temp_pos += (buffer[counter] - '0');
                                break;
                            } else {
                                break;
                            }
                        }
                    }
                break;
            }
            ++counter;
        }
        fputc('0' + temp_coord.dir, inverted_out);
        fputc('-', inverted_out);
        //printf("going to %lld\n", temp_pos);
        //printf("%d-%d-", temp_coord.dir, temp_pos);
        fseek(queue, temp_pos, SEEK_SET);
    }

    fputc(in_cord.dir + '0', inverted_out);

    //And now, reverse the order and start constructing!

    int forward_length = -1;
    byte last_dir = in_cord.dir;
    byte current_dir = in_cord.dir;
    char ch;
    fseek(inverted_out, -1, SEEK_END);
    do {
        ch = fgetc(inverted_out);
        fputc(ch, unconverted_out);
        fseek(inverted_out, -2, SEEK_CUR);
    } while (ftell(inverted_out) != 0L);
        ch = fgetc(inverted_out);
        fputc(ch, unconverted_out);

    fclose(inverted_out);
    fseek(unconverted_out, 0, SEEK_SET);

    sprintf(buffer, " "); //Purge excess buffer data (IMPORTANT!)

    while ((ch = fgetc(unconverted_out)) != EOF) {
        if (ch != '-') {
            current_dir = ch - '0';
            if (current_dir != last_dir) {
                if (forward_length > 0) {
                    counter = 0;
                    sprintf(buffer, "FORWARD %d\n", forward_length);
                    fwrite(buffer, 1, (size_t)strlen(buffer), actual_out);
                }
                // p+d % 4 = albo 1, albo 3
                // jeśli p < d, i (%) = 1, R
                // jeśli p < d, i (%) = 3, L

                //jeśli p > d i (%) = 1, L
                //jeśli p > d i (%) = 3, R
                if ((last_dir + current_dir) % 4 == 1) ((last_dir < current_dir) ? sprintf(buffer, "TURN RIGHT\n") : sprintf(buffer, "TURN LEFT\n"));
                else ((last_dir < current_dir) ? sprintf(buffer, "TURN LEFT\n") : sprintf(buffer, "TURN RIGHT\n") ); 
                fwrite(buffer, 1, (size_t)strlen(buffer), actual_out);

                last_dir = current_dir;
                forward_length = 0;
            }
            ++forward_length;
        }
    }
    if (forward_length > 0) {
        snprintf(buffer, 160, "FORWARD %d\n", forward_length);
        fwrite(buffer, 1, (size_t)strlen(buffer), actual_out);
    }

    fclose(inverted_out);
    fclose(unconverted_out);

    rewind(actual_out);
    return actual_out;
    //0-3-0-0-1
    // Mniejszy w lewo, większy w prawo
}

FILE* bfs_runner(byte maze_build[][256], byte maze_nodes[][128], bit_pair maze_size, maze_cord in_cord, maze_cord out_cord) {
    char queue_buffer[176]; //Buffer for a single line in my queue (including padding for '\n' and '\0')
    long long queue_bin_pos, queue_parent_pos = -1; //Long long because of how large the queue file could get.
    FILE* queue = tmpfile(); //tmpfile(); //Opens in read + write mode. The queue for my BFS.

    byte info_byte = 0x00;
    maze_cord running_cord = in_cord;

    /*for (int i = 0; i < maze_size.y; ++i) {
        for (int j = 0; j <= maze_size.x / 4; ++j) {
            read_bits(maze_build[i][j]);
        }
        printf("\n");
    }*/
    
    //fprintf(queue, "%d %d %d:%ld\n", in_cord.x, in_cord.y, in_cord.dir, queue_bin_pos);
    while(unfound(running_cord, out_cord)) {
        info_byte = construct_info_byte(maze_build, maze_nodes, running_cord, maze_size); //Build an info byte,
        queue_bin_pos = ftell(queue); //Remember where we are in our queue,
        fseek(queue, 0, SEEK_END); //(NOT PORTABLE! Should? be fine) Go to the end of the queue,
        enqueue(queue, maze_nodes, running_cord, info_byte, queue_parent_pos, queue_buffer); //Enqueue based on the byte (this could be one step with info_byte, but...),
        if(ftell(queue) == queue_bin_pos) {
            printf("Giving up! No path seems to exist\n");
            break;
        }
        running_cord = dequeue(queue, &queue_parent_pos, queue_bin_pos, queue_buffer); //Load current queue pos into memory,
        //And keep going.

        //I'll reconstruct afterwards thanks to queue_parent_pos!
    }

    FILE* out;
    out = reconstruct_path(queue, in_cord, out_cord, queue_bin_pos, queue_buffer); //The output file - a list of steps needed

    return out;
}