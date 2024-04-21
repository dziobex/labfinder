#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "bitish.h"
#include "dencoder.h"

maze_cord make_coord(unsigned short x, unsigned short y, byte dir);
//Makes a maze_cord.

byte valid_coords(maze_cord checked_pos, bit_pair maze_size);
//Checks if a position being accessed is valid.

byte construct_info_byte(byte maze_build[][256], byte maze_nodes[][128], maze_cord maze_pos, bit_pair maze_size);
//Creates a byte containing the info of if the 4 directions can be accessed, as well as if they've already been checked

void enqueue(FILE* queue, maze_cord maze_pos, byte info_byte, long long queue_bin_pos);
//Adds every direction that has not yet been checked to the queue. This has to append to the file..?

maze_cord dequeue(FILE* queue, long long queue_bin_pos, char* buffer[]);
//Gets the position from position 'queuepos' in the queue.

FILE* reconstruct_path(FILE* queue, maze_cord cur_pos, long long queue_bin_pos);
//Rebuilds the path taken by adding it to a file.

FILE* bfs_runner(byte maze_build[][256], byte maze_nodes[][128], bit_pair maze_size, maze_cord in_cord, maze_cord out_cord);
// Rolls a breadth-first search. Returns a FILE with the correct steps to solve it, in order.
