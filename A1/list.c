/*
 * list.c
 *
 * Based on the implementation approach described in "The Practice 
 * of Programming" by Kernighan and Pike (Addison-Wesley, 1999).
 * 
 * Re-used from Seng265 lab07 in Winter 2021 at the University of Victoria, taught by Hausi Muller and completed by Paul Garewal. 
 * 
 * 
 * 
 */

#include <assert.h>
#include <string.h>
#include "emalloc.h"
#include "list.h"
#include <unistd.h>     // fork(), execvp()
#include <stdio.h>      // printf(), scanf(), setbuf(), perror()
#include <stdlib.h>     // malloc()
#include <sys/types.h>  // pid_t 
#include <sys/wait.h>   // waitpid()
#include <signal.h>     // kill(), SIGTERM, SIGKILL, SIGSTOP, SIGCONT
#include <errno.h>      // errno

/**
 * @brief Function: newNode
 * 
 *        Description: Creates new process node
 *                       
 * 
 * @param pid_t - pid of process
 * @param char* - current path of process
 * 
 *
 */
node_t *new_node(pid_t pid, char* path) {
    node_t *temp = (node_t *)malloc(sizeof(node_t));

    assert(temp != NULL);  /* If this goes wrong, stop the train. */

    if(path == NULL){
        temp->pid = pid;
        temp->path = "NULL";
        temp->next = NULL;

        return temp;
    }

    temp->pid = pid;
    temp->path = strdup(path);
    temp->next = NULL;

    return temp;
}


/**
 * @brief Function: add_end
 * 
 *        Description: adds node to linked list of processes.
 *                       
 * 
 * @param node_t - list of nodes
 * @param node_t - node of process to add to linked list
 *
 */


node_t *add_end(node_t *list, node_t *new) {
    node_t *curr;

    if (list == NULL) {
        list = new;
        return list;
    }

    for (curr = list; curr->next != NULL; curr = curr->next);
    curr->next = new;
    new->next = NULL;
    list = curr;
    return list;
}
/**
 * @brief Function: deleteNode
 * 
 *        Description: deletes node in linked list of processes.
 *                       
 * 
 * @param node_t - list of nodes
 * @param pid_t - pid of process to delted from linked list
 * 
 *
 */

node_t * deleteNode(node_t *list, pid_t pid){
    node_t *curr = list;

    node_t *prev = NULL;

    if(list == NULL){
        return list;
    }

    if(list->next == NULL && list->pid == pid){
        free(list);
        list = NULL;
        return list;
    }

    while(curr != NULL && curr->pid != pid){

        prev = curr;
        curr = curr->next;

    }

    if(curr == NULL){
        return curr;
    }
    prev->next = curr->next;

    free(curr);

    return list;
}

