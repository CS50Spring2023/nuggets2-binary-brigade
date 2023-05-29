#
# Makefile for 'server'
# CS50 project 'Nuggets'
#
# Binary Brigade, Spring, 2023
#

SUPPORT_DIR = ../support
LIBS = -lncurses
LLIBS = support/support.a

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$(SUPPORT_DIR) -I../game -I../grid -I../player -I../file -I../game -I../lib

.PHONY: all clean

all: server

server: server.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

##################### Dependencies #####################

server.o: server.c $(SUPPORT_DIR)/message.h ../game/game.h ../grid/grid.h ../player/player.h
	$(CC) $(CFLAGS) -c $< -o $@

message.o: $(SUPPORT_DIR)/message.c $(SUPPORT_DIR)/message.h

../grid/grid.o: ../grid/grid.h ../file/file.h ../lib/mem.h ../player/player.h ../game/game.h

../player/player.o: ../player/player.h ../grid/grid.h $(SUPPORT_DIR)/message.h

../game/game.o: ../game/game.h ../grid/grid.h ../player/player.h ../lib/mem.h

############# clean ###########
clean:
	rm -f core
	rm -rf *~ *.o *.gch *.dSYM
	rm -f *.log
	rm -f $(LIB)
	rm -f $(TESTS)
	rm -f *.o server
