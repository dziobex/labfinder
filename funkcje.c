#include <stdio.h>
#include <stdlib.h>

// sprawdzenie czy labirynt z pliku jest DOBRY
// czyli: 1 in, 1 out, wszystkie wiersze/kolumny mają taki sam rozmiar dla swojego typu

/*
    zwracane wartości:
    0 - brak błędu
    1 - nierówna liczba wierszy/kolumn: zmutowany labirynt!
    2 - brak lub więcej niż 1 punkt wyjścia/wejścia
    3 - niedozwolone rozmiary labiryntu: za mały/duży
*/

char valid_lab( FILE* in ) {
    short row_size = 0, col_size = 0, cs = 0;   // 3*2
    char in_count = 0, out_count = 0, ch;       // 3

    fseek(in, 0, SEEK_SET);

    while ( (ch  = fgetc(in)) != EOF ) {
        if ( ch == '\n') {
            if ( cs != 0 && cs != col_size )
                return 1;                       // nierówna liczba wierszy/kolumn
            row_size++;
            cs = 0;
        }
        else if ( row_size == 0 )
            col_size++;                         // macierzysta liczba kolumn - z nią są porównywane kolejne liczby kolumn
        else
            cs++;

        if ( ch == 'P' )                        // in
            ++in_count;
        else if ( ch == 'K' )                   // out
            ++out_count;
    }

    if ( cs != 0 && cs != col_size )
        return 1;                               // ostatni wiersz nie jest porównywany przez wystąpienie EOFa

    if ( in_count != 1 || out_count != 1)
        return 2;                               // nieprawidłowa liczba in/outów

    if ( row_size < 5 || row_size > 1024 || col_size < 6 || col_size > 1025 )
        return 3;                               // niedozwolony rozmiar labiryntu

    return 0;
}

// zwraca rozmiar labiryntu: wiersze (r), kolumny (c) - zakładamy, że labirynt nie jest popsuty (sprawdzenie nastąpiło w funkcji wyżej)

void get_size( FILE* in, short* r, short* c ) {
    fseek(in, 0, SEEK_SET);

    char ch;
    *c = 0;
    *r = 0;

    while ( (ch  = fgetc(in)) != EOF )
        if ( ch == '\n')
            (*r)++;
        else if ( *r == 0 )
            (*c)++;

    (*c)--;                                     // nieparzysta liczba
}

// kopiowanie labiryntu do pliku tymczasowego, na którym będzie znajdowana jakakolwiek ścieżka do wyjścia
// ten plik będzie usuwany po zakończeniu działania programu (nie będzie też nadpisywać żadnego istniejącego pliku)

FILE* copy_file( FILE* src ) {
    FILE* dest;
    dest = tmpfile();

    char c;
    while( ( c = fgetc(src) ) != EOF )
        fputc(c, dest);

    return dest;
}

int main ( int argx, char **argv ) {

    // małe testy

    FILE* in = fopen("samples/01.txt", "r+");

    FILE* test = copy_file(in);

    char v = valid_lab( test );
    printf("Labirynt jest ");

    if ( v == (char)0 )
        printf("poprawny 8)\n");
    else {
        printf( "niepoprawny 8( kod bledu: %c\n", v+'0');
        return 1;
    }

    short* r = malloc(sizeof( short ));
    short* c = malloc(sizeof( short ));

    get_size(test, r, c);

    printf("rows count: %hd\ncolumns count: %hd\n", *r, *c );

    free(in);
    free(test);

    free(r);
    free(c);

    return 0;
}