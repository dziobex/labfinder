CC=gcc

labfinder: main.c encode.o decode.o exit_codes.h
	$(CC) -Wall main.c encode.o decode.o -o labfinder

encode.o: encoder.c dencoder.h bits.h exit_codes.h
	$(CC) -Wall -c encoder.c -o encode.o

decode.o: decoder.c dencoder.h bits.h exit_codes.h
	$(CC) -Wall -c decoder.c -o decode.o