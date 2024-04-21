CC=gcc

labfinder: main.c encode.o decode.o mazerunner.o exit_codes.h
	$(CC) -Wall main.c encode.o decode.o mazerunner.o -o labfinder

encode.o: encoder.c dencoder.h bitish.h exit_codes.h 
	$(CC) -Wall -c encoder.c -o encode.o

decode.o: decoder.c dencoder.h bitish.h exit_codes.h
	$(CC) -Wall -c decoder.c -o decode.o

mazerunner.o: mazerunner.c dencoder.h bitish.h exit_codes.h mazerunner.h
	$(CC) -Wall -c mazerunner.c -o mazerunner.o