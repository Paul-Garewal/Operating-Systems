/*
 * emalloc.c
 *
 * Based on the implementation approach described in "The Practice 
 * of Programming" by Kernighan and Pike (Addison-Wesley, 1999).
 * 
 * Re-used from Seng265 lab07 in Winter 2021 at the University of Victoria, taught by Hausi Muller and completed by Paul Garewal. 
 * 
 * 
 * 
 */


#include <stdlib.h>
#include <stdio.h>
#include "emalloc.h"


void *emalloc(size_t n) {
    void *p; 

    p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "malloc of %zu bytes failed", n); 
        exit(1);
    }   

    return p;
}
