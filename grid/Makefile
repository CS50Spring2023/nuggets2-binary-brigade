# Makefile for testing the grid module
# CS50 Project
# Louis Weisdorf

OBJS = grid.o mem.o file.o
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING)
CC = gcc
MAKE = make

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

.PHONY: test valgrind clean

grid: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

grid.o: grid.c grid.h ../lib/mem.h ../lib/file.h
	$(CC) $(CFLAGS) -c grid.c -o grid.o -I.

mem.o: ../lib/mem.c ../lib/mem.h
	$(CC) $(CFLAGS) -c ../lib/mem.c -o mem.o

file.o: ../lib/file.c ../lib/file.h
	$(CC) $(CFLAGS) -c ../lib/file.c -o file.o

test: grid
	./grid

valgrind: grid
	$(VALGRIND) ./grid

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f grid
	rm -f core