# #
# # Makefile for 'client'
# # CS50 project 'Nuggets'
# #
# # Binary Brigade, Spring, 2023
# #

# LIB = 
# S = ../support
# LIBS = -lncurses
# LLIBS = $(S)/support.a
# TESTS = client

# CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$(S)
# CC = gcc
# MAKE = make

# .PHONY: all clean

# ############# default rule ###########
# all: $(LIB) $(TESTS) 

# $(LIB): $(S)/message.o log.o
# 	ar cr $(LIB) $^

# client: client.o $(LLIBS)
# 	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# client.o: $(S)/message.h $(S)/log.h 



# ############# clean ###########
# clean:
# 	rm -f core
# 	rm -rf *~ *.o *.gch *.dSYM
# 	rm -f *.log
# 	rm -f $(LIB)
# 	rm -f $(TESTS)


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

# Targets
all: client

client: client.o $(SUPPORT_DIR)/message.o $(SUPPORT_DIR)/log.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

client.o: client.c $(SUPPORT_DIR)/message.h $(SUPPORT_DIR)/log.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SUPPORT_DIR)/message.o: $(SUPPORT_DIR)/message.c $(SUPPORT_DIR)/message.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SUPPORT_DIR)/log.o: $(SUPPORT_DIR)/log.c $(SUPPORT_DIR)/log.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o client
