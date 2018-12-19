#ifndef SIMBOLOS_H
#define SIMBOLOS_H

#include "lexico.h"
#include "vector.h"

typedef struct{
    char* nome;
    char categoria, tipo, scopo;
}symbol;

//myvector new_vector(symbol*, int);
void push_back_symbol(symbol*);
void pop_until_param();
symbol *symbol_at(int);
void alter_symbol_at(int, symbol*);

int find_function_procudure();
int find_function_procudure_signature();
int find_value_by_name(char*, int);
int find_value_by_name_local(char*, int);
int find_variable_by_name(char*);
int find_function_by_name(char*);
int find_procedure_by_name(char*);
int find_param_by_name(char*, int);
int find_no_param_by_name(char*);
int find_until_function_by_name(char*);

symbol *new_symbol(char*, int, int, int);
bool cmp_symbol(symbol, symbol);
void free_symbol(symbol);
void free_table();
void print_table();

#endif