#
# Makefile for 'library.a'
# CS50 project 'Nuggets'
#
# David Kotz, May 2019, 2021
#

LIB = library.a
TESTS =

CFLAGS = -Wall -pedantic -std=c11 -ggdb
CC = gcc
MAKE = make

.PHONY: all clean

############# default rule ###########
all: $(LIB) $(TESTS) 

$(LIB): mem.o file.o
	ar cr $(LIB) $^



############# clean ###########
clean:
	rm -f core
	rm -rf *~ *.o *.gch *.dSYM
	rm -f *.log
	rm -f $(LIB)
	rm -f $(TESTS)
