//
//  queue.h
//  Matzov-C
//
//  Created by Gabriel Kapach on 15/09/2017.
//  Copyright © 2017 Gabriel Kapach. All rights reserved.
//

#ifndef queue_h
#define queue_h

#include <stdio.h>
#include <stdlib.h>

#endif /* queue_h */

typedef struct node {
    int value;
    struct node* prev;
} node;

typedef struct queue {
    node *head;
    node *tail;
    int count;
} queue;

node* create_node(int value);
queue* queue_init(int value);
void queue_destory(node* head);
node* enqueue(queue* self, int value);
int dequeue(queue* self);
int queue_count(queue* self);
int queue_next_value(queue* self);