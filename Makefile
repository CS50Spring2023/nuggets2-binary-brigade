#
# Makefile for 'server'
# CS50 project 'Nuggets'
#
# Binary Brigade, Spring, 2023
#

SUPPORT_DIR = support
LIBS = -lncurses -lm 
LLIBS = support/support.a lib/library.a

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -Isupport  -Ilib

.PHONY: all clean client

all: library support/support.a server/server client
	

server/server: server/server.o $(SUPPORT_DIR)/message.o grid/grid.o player/player.o game/game.o 
	$(CC) $(CFLAGS) $^  $(LLIBS) $(LIBS) -o $@

server.o: server.c $(SUPPORT_DIR)/message.h game/game.h grid/grid.h player/player.h lib/mem.h support/log.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SUPPORT_DIR)/message.o: $(SUPPORT_DIR)/message.c $(SUPPORT_DIR)/message.h
	$(CC) $(CFLAGS) -c $< -o $@

game/game.o: game/game.c game/game.h grid/grid.h player/player.h lib/mem.h
	$(CC) $(CFLAGS) -c $< -o $@

grid/grid.o: grid/grid.c grid/grid.h lib/file.h lib/mem.h
	$(CC) $(CFLAGS) -c $< -o $@

player/player.o: player/player.c player/player.h grid/grid.h $(SUPPORT_DIR)/message.h lib/mem.h 
	$(CC) $(CFLAGS) -c $< -o $@

library: 
	make -C lib

support/support.a:
	make -C $(SUPPORT_DIR)

client:
	$(MAKE) --directory=client

clean:
	rm -f core
	rm -rf *~ *.o *.gch *.dSYM
	rm -f *.log
	rm -f server/server
	rm -f server/server.o
	rm -f game/game.o
	rm -f grid/grid.o
	rm -f player/player.o
	make --directory=client clean
	make --directory=support clean
	make --directory=lib clean



