//
//  queue.c
//  Matzov-C
//
//  Created by Gabriel Kapach on 15/09/2017.
//  Copyright © 2017 Gabriel Kapach. All rights reserved.
//
#include "queue.h"

/*
 * Function: create_node
 * ---------------------
 * Returns a pointer for a new queue node
 *
 * Value: the value of the new node
 *
 */

node* create_node(int value) {
    node *tmp = (node*) malloc(sizeof(node));
    
    if (tmp == NULL) {
        printf("\n Node creation failed \n");
        return NULL;
    }
    
    tmp->value = value;
    
    return tmp;
}

/*
 * Function: queue_init
 * --------------------
 * Returns a new queue with one node
 *
 * value: the value of the first node
 *
 */

queue* queue_init(int value) {
    queue* self = (queue*) malloc(sizeof(queue));
    
    if (self == NULL) {
        printf("\n Queue creation failed \n");
        exit(0);
    }
    
    // Create head node
    self->head = create_node(value);
    
    // Set the new queue data
    if (self->head) {
        self->count = 1;
        self->tail = self->head;
    }
    
    return self;
}

/*
 * Function: queue_destory
 * -----------------------
 * Setting free all the memory which was allocated for the queue
 *
 * head: the head node of a queue
 *
 */

void queue_destory(node* head) {
    node* tmp = head->prev;
    
    if (tmp) {
        queue_destory(tmp);
    }
    
    free(head);
}

/*
 * Function: enqueue
 * -----------------
 * Append a new node to the queue
 *
 * self: reference to the queue you're working on
 * value: value for the new node
 *
 * returns: the new node pointer
 */

node* enqueue(queue* self, int value) {
    node *tmp = create_node(value);
    
    if (tmp) {
        self->tail->prev = tmp;
        self->tail = tmp;
        
        self->count++;
    }
    
    return tmp;
}

/*
 * Function: dequeue
 * -----------------
 * Returns the value of first node of the queue and removed it
 *
 * self: reference to the queue you're working on
 *
 */

int dequeue(queue* self) {
    int value = self->head->value;
    node* tmp = self->head->prev;
    
    free(self->head);
    self->head = tmp;
    self->count--;
    
    return value;
}

/*
 * Function: queue_count
 * -----------------
 * Returns the count of node in the queue
 *
 * self: reference to the queue you're working on
 *
 */

int queue_count(queue* self) {
    return self->count;
}

/*
 * Function: queue_count
 * -----------------
 * Returns the value of first node of the queue without removing it
 *
 * self: reference to the queue you're working on
 *
 */

int queue_next_value(queue* self) {
    return self->head->value;
}
