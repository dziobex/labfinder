#include <stdio.h>
#include <stdlib.h>
#include "parsec.h"

/* long* grab_even_pos(FILE* in, short* c) {
    long* position_array = malloc(sizeof(long) * (*c));
    char ch, flag = 's'; //Flag has 3 states, n for not yet, s for soon, y for yes
    short re_pos = 0;

    rewind(in);
    while((ch = fgetc(in)) != EOF) {
        if (flag == 'y') {
            position_array[re_pos] = ftell(in);
            ++re_pos;
            flag = 'n';
        }
        else if(ch == '\n') {
            switch(flag) {
                case 's':
                    flag = 'y';
                    break;
                case 'n':
                    flag = 's';
                    break;
            }
        }
    }

    return position_array;
} */

/* 
Code above is obsoleted and kept as a comment. This can be skipped - we can get exact binary pos in the file via calculations
each row has 2*(r+1) length, thanks to tmpfile opening a binary.
so to get to (x,y), we can move exactly (y * 2 * (r+1)) + (x*2) from the start
the thing is, of course, this would move over the entire thing, and not the points we WANT it to move to
so instead, it's something like... ((2y+1) * 2 * (r+1)) + ((2x+1)*2), presumably?

how about the other way around?
given a byte position of, let's say 1024 for a 512x1024 maze, we know we're on the 512th char somewhat?
hopefully linux vs windows doesn't mess us up here, or this won't be portable and it's back to pos arrays
*/

coordinate parametrize(FILE* in, short* r) {
    coordinate xy;
    long bytepos = ftell(in); 
    short course_corrector = 0;

    while (bytepos / (2* (*r) + 2) > bytepos % (2* (*r) + 2)) {
        bytepos -= (2* (*r) + 2);
        ++course_corrector;
    }

    xy.x = (bytepos % (2* (*r) + 2)) + course_corrector*256;
    xy.y = 0;
    //not expecting this to work yet
    printf("got position %ld (%d,%d)\n", bytepos, xy.x, xy.y);
    return xy;
}
/*THIS DOESN'T WORK. MY PREVIOUS IDEA WORKED FINE BEFORE X=256.
SEEMS THE BYTE MULTIPLIES ENOUGH TO CYCLE BETWEEN 0 AND 255 WHEN MOD 1026 FOR THIS CASE.
THERE'S GOT TO BE A WAY TO FIX THIS. I DON'T HAVE TIME ATM. SADLY.
POSITION SHIFTING WORKS PERFECTLY FINE.
*/

void shift_pos(FILE *in, coordinate xy, short* r) {
    long bytepos = (2*xy.y + 1) * 2 * (*r + 1);
    bytepos += (2*xy.x + 1) * 2;
    if (fseek(in, bytepos, SEEK_SET) != 0) printf("bad news - fseek errored");
    printf("posisiton shifted to %ld, represents (%d,%d)\n", ftell(in), xy.x, xy.y);
}