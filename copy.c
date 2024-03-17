#include<stdio.h>    
#include<stdlib.h>

void copy_file(FILE *src, FILE *dest) {
    char c;
    while( ( c = fgetc(src) ) != EOF )
        fputc(c, dest);
}

int main (int argc, char  *argv[]) {

}