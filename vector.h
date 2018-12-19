#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct myvector{
    int tam, current;
    void **value;
}myvector;

myvector *new_vector(int);
void push_back(myvector*, void*);
void pop_back(myvector*);
void *value_at(myvector*, int);
void alter_at(myvector*, int, void*);
void free_vector(myvector*);
void err(char *);
#endif