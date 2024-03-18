#pragma once
#include <stdio.h>

typedef struct coordinate_t {
    unsigned short x;
    unsigned short y;
} coordinate;

//Returns (x,y) coordinates of iterator in the file. 
coordinate parametrize(FILE* in, short* r);
//Shifts the file pos indicator to position (x,y)
void shift_pos(FILE *in, coordinate xy, short* r); 

// long* grab_even_pos(FILE* in, short* c); //Gets the positions of the first char in all even lines as an int. UNNecessary preprocessing. 