

#Makefile
 
#Based on the implementation approach described in "The Practice of Programming" by Kernighan and Pike (Addison-Wesley, 1999).
 
#Re-used from Seng265 lab07 in Winter 2021 at the University of Victoria, taught by Hausi Muller and completed by Paul Garewal. 
 

#.phony all:
#all: PMan

#man: main.c linked_list.c
#	gcc -Wall main.c linked_list.c -o PMan

#.PHONY clean:
#clean:
CC=gcc

# The line with -DDEBUG can be used for development. When
# building your code for evaluation, however, the line *without*
# the -DDEBUG will be used.
#

CFLAGS=-c -Wall -g -DDEBUG -O0
//-std=c99

all: PMan


PMan: PMan.o list.o emalloc.o
	$(CC) PMan.o list.o emalloc.o -o PMan

PMan.o: PMan.c list.h emalloc.h
	$(CC) $(CFLAGS) PMan.c

list.o: list.c list.h emalloc.h
	$(CC) $(CFLAGS) list.c

emalloc.o: emalloc.c emalloc.h
	$(CC) $(CFLAGS) emalloc.c

clean:
	rm -rf *.o PMan
