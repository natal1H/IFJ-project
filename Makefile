
CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -lm

all: scanner

scanner: scanner.c
	$(CC) $(CFLAGS) scanner.c stringlib.c -o scanner

clean:
	rm -f scanner
