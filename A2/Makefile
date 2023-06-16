

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

CFLAGS=-c -Wall -g -pthread -DDEBUG -O0
//-std=c99

all: ACS


ACS: ACS.o queue.o emalloc.o
	$(CC) ACS.o queue.o emalloc.o -o ACS

ACS.o: ACS.c queue.h emalloc.h
	$(CC) $(CFLAGS) ACS.c

queue.o: ACS.c queue.h emalloc.h
	$(CC) $(CFLAGS) queue.c

emalloc.o: emalloc.c emalloc.h
	$(CC) $(CFLAGS) emalloc.c

clean:
	rm -rf *.o ACS
