# Very simple Makefile
CFLAGS=-Wall -ansi -pedantic

bf-interpreter: bf-interpreter.c
	gcc $(CFLAGS) -o $@ $<
