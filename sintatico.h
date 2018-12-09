#ifndef SINTATICO_H
#define SINTATICO_H

#include "lexico.h"

enum simcat{ VARIAVEL, FUNCTION, PROCEDURE, PARAM };

typedef struct{
    char* nome;
    char categoria, tipo, scopo;
}simbolo;

typedef struct item{
    simbolo *valor;
}item;

typedef struct myvector{
    int tam, current;
    item *value;
}myvector;

myvector new_vector(simbolo*, int);
void push_back(myvector, item);
void pop_back(myvector);
item value_at(myvector, int);
item *find_value(myvector, item);
bool cmp_item(item, item);
void free_item(item);

bool attribution();
bool expr();
bool op_rel();
bool simple_expr();
bool term();
bool factor();
bool procedure();
bool function();
bool cmd();

bool param();
bool declaration();
bool identifier();
bool type();
bool ct_int();
bool ct_car();
bool ct_string();
bool ct_real();

void start();
void prog();
void var();

char t_char();
int t_int();
float t_float();
char *t_string();


bool get_next();
void analise();

void error(int);
#endif