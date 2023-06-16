/*
 * list.h
 *
 * Based on the implementation approach described in "The Practice 
 * of Programming" by Kernighan and Pike (Addison-Wesley, 1999).
 * 
 * Re-used from Seng265 lab07 in Winter 2021 at the University of Victoria, taught by Hausi Muller and completed by Paul Garewal. 
 * 
 * 
 * 
 */


#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#define MAX_WORD_LEN  50
#include <sys/types.h>  // pid_t 
typedef struct node_t {
    pid_t            pid;
    char*             path;
    struct node_t  *next;
} node_t;

node_t *new_node(pid_t pid, char* path);

node_t *add_end(node_t *, node_t *);

node_t *deleteNode(node_t *, pid_t pid);

void    apply(node_t *, void(*fn)(node_t *, void *), void *arg);

#endif
