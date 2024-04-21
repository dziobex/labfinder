#include <stdio.h>
#include <stdlib.h>

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
    if (cur.x == want.x && cur.y == want.y - 1) return 0;
    return 1;
} //make this a ternary lol

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
    printf("\nEntered construct, pos is %d %d\n", maze_pos.x, maze_pos.y);
    //While a loop might be possible with an eval, too lazy lol

    //Checks down
    if(valid_coords(make_coord(maze_pos.x, maze_pos.y + 1, maze_pos.dir), maze_size)) {
        printf("Down is a valid coordinate!\n");
        printf("Looking for byte %d of %d %d (", (7 - (maze_pos.x) % 8), maze_pos.y+1, maze_pos.x / 8);
        for (short dds = 7; dds >= 0; --dds) {
            printf("%d", GETBIT(maze_nodes[maze_pos.y+1][maze_pos.x / 8], dds));
        }
        printf(")\n");
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

    for (byte dsds = 0; dsds < 8; ++dsds) {
        printf("%d", GETBIT(info_byte, dsds));
    }
    printf("\n");
    return info_byte;
}

void enqueue(FILE* queue, byte maze_nodes[][128], maze_cord maze_pos, byte info_byte, long long queue_bin_pos) {
    for(byte i = 0; i < 4; ++i) {
        if(GETBIT(info_byte, i) == 0 && GETBIT(info_byte, i+4) == 0) {
            switch(i) {
                case 0:
                    fprintf(queue, "%d %d 0:%ld\n", maze_pos.x, maze_pos.y + 1, queue_bin_pos);
                    break;
                case 1:
                    fprintf(queue, "%d %d 3:%ld\n", maze_pos.x + 1, maze_pos.y, queue_bin_pos);
                    break;
                case 2:
                    fprintf(queue, "%d %d 2:%ld\n", maze_pos.x, maze_pos.y - 1, queue_bin_pos);
                    break;
                case 3:
                    fprintf(queue, "%d %d 1:%ld\n", maze_pos.x - 1, maze_pos.y, queue_bin_pos);
                    break;
            }
        }
    }
}

maze_cord dequeue(FILE* queue, long long queue_bin_pos, char* buffer) {
    printf("at %ld right before fseek \n", ftell(queue));
    fseek(queue, queue_bin_pos, SEEK_SET);
    printf("at %ld right after fseek \n", ftell(queue)); // There we go. So this says it doesn't reseek properly.
    fgets(buffer, 94, queue); //Why is this getting everything???
    printf("at %ld while dequeueing \n", ftell(queue));

    maze_cord return_value = make_coord(0, 0, 0x00);

    //Yep, I'm making my own atoi. Why?
    //Several 'ints' i need to split. I don't feel like thinking about the headache too much.

    byte divOccurs = 0;
    for (byte counter = 0; counter < sizeof(buffer); ++counter) {
        switch(buffer[counter]) {
            case ':':
                break; //I can do this, because the : means i'm entering the queue position. I don't need it for dequeueing,
                //but I'll need it way later instead, for backtracking. Either way, : guaranteed to appear before \n or \0
            case ' ':
                ++divOccurs;
                break;
            default:
                switch(divOccurs) {
                    case 0:
                        return_value.x *= 10;
                        return_value.x += (buffer[counter] - '0');
                        break;
                    case 1:
                        return_value.y *= 10;
                        return_value.y += (buffer[counter] - '0');
                        break;
                    case 2:
                        return_value.dir = buffer[counter] - '0'; // Direction is guaranteed to be one digit.
                        ++divOccurs; //Putting us into pure breaks.
                        break;
                    default:
                        break;
                }
                break;
        }
    }

    fputs(buffer, stdout);

    return return_value;
}

FILE* reconstruct_path(FILE* queue, maze_cord cur_pos, long long queue_bin_pos) {

}

FILE* bfs_runner(byte maze_build[][256], byte maze_nodes[][128], bit_pair maze_size, maze_cord in_cord, maze_cord out_cord) {
    char queue_buffer[96]; //Buffer for a single line in my queue (including padding for '\n' and '\0')
    long long queue_bin_pos; // Might just swap to this? Queue position for the file to understand
    FILE* queue = fopen("queue.txt", "wb+"); //tmpfile(); //Opens in read + write mode. The queue for my BFS.
    FILE* out = tmpfile(); //The output file - a list of steps needed

    byte info_byte = 0x00;
    maze_cord running_cord = in_cord;

    short i = 0;
    
    for (int i = 0; i < maze_size.x; ++i) {
        for (int j = 0; j < (maze_size.y / 4) + 1; ++j) {
            printf(" ");
            for(int b = 7; b >= 0; --b) {
                printf("%d", GETBIT(maze_build[i][j], b));
            }
        }
        printf("\n");
    }
    //fprintf(queue, "%d %d %d:%ld\n", in_cord.x, in_cord.y, in_cord.dir, queue_bin_pos);
    while(unfound(running_cord, out_cord)) {
        info_byte = construct_info_byte(maze_build, maze_nodes, running_cord, maze_size); //Build an info byte,
        queue_bin_pos = ftell(queue); //Remember where we are in our queue,
        printf("at %ld in file\n", queue_bin_pos);
        fseek(queue, 0, SEEK_END); //(NOT PORTABLE! Should? be fine) Go to the end of the queue,
        enqueue(queue, maze_nodes, running_cord, info_byte, queue_bin_pos); //Enqueue based on the byte (this could be one step with info_byte, but...),
        printf("at %ld vs %ld before dequeue\n", queue_bin_pos, ftell(queue));
        running_cord = dequeue(queue, queue_bin_pos, queue_buffer); //Load current queue pos into memory,
        //And keep going.

        for (short fvd = 0; fvd < 3; ++fvd) {
            for (short dds = 7; dds >= 0; --dds) {
                printf("%d", GETBIT(maze_nodes[fvd][0], dds));
            }
            printf("\n");
        }

        printf("%d %d vs %d %d\n", running_cord.x, running_cord.y, out_cord.x, out_cord.y);

        //I'll reconstruct afterwards thanks to directions
    }

    out = reconstruct_path(queue, running_cord, queue_bin_pos);

    return out;
}