#
# Makefile for 'client'
# CS50 project 'Nuggets'
#
# Binary Brigade, Spring, 2023
#

SUPPORT_DIR = ../support
LIBS = -lncurses

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$(SUPPORT_DIR)

.PHONY: all clean

all: client

client: client.o $(SUPPORT_DIR)/message.o $(SUPPORT_DIR)/log.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

client.o: client.c $(SUPPORT_DIR)/message.h $(SUPPORT_DIR)/log.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SUPPORT_DIR)/message.o: $(SUPPORT_DIR)/message.c $(SUPPORT_DIR)/message.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SUPPORT_DIR)/log.o: $(SUPPORT_DIR)/log.c $(SUPPORT_DIR)/log.h
	$(CC) $(CFLAGS) -c $< -o $@

############# clean ###########
clean:
	rm -f core
	rm -rf *~ *.o *.gch *.dSYM
	rm -f *.log
	rm -f $(LIB)
	rm -f $(TESTS)
	rm -f *.o client
	rm -f *.log
