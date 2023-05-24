#
# Makefile for 'client'
# CS50 project 'Nuggets'
#
# Binary Brigade, Spring, 2023
#

LIB = 
S = ../support
LIBS = -lncurses
LLIBS = $(S)/support.a
TESTS = client

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$(S)
CC = gcc
MAKE = make

.PHONY: all clean

############# default rule ###########
all: $(LIB) $(TESTS) 

$(LIB): $(S)/message.o log.o
	ar cr $(LIB) $^

client: client.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

client.o: $(S)/message.h $(S)/log.h 



############# clean ###########
clean:
	rm -f core
	rm -rf *~ *.o *.gch *.dSYM
	rm -f *.log
	rm -f $(LIB)
	rm -f $(TESTS)
