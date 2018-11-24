CC=gcc
#CFLAGS=-Wall -std=c99 -pedantic -lm

all: main

main: main.c parser.c scanner.c stringlib.c expression_parser.c symtable.c semantic_analysis.c
	$(CC) main.c parser.c scanner.c stringlib.c expression_parser.c symtable.c semantic_analysis.c -o main

clean:
	rm -f main