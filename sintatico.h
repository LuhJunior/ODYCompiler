#ifndef SINTATICO_H
#define SINTATICO_H

#include "lexico.h"

typedef struct item{
    void *valor;
}item;

typedef struct myvector{
    int tam, current;
    item *value;
}myvector;

myvector new_vector(void *, int);
void push_back(myvector, void *);
void pop_back(myvector);
item value_at(myvector);
item *find_value(myvector, item);
bool cmp_item(item, item);
void free_item(item);

int cmd();
void error(int);
bool get_next();
void analise();

#endif