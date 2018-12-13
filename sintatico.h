#ifndef SINTATICO_H
#define SINTATICO_H

#include "lexico.h"

enum simcat{ VARIAVEL, FUNCTION, PROCEDURE, FUNCTION_SIGNATURE, PROCEDURE_SIGNATURE, PARAM };

enum escopo{ GLOBAL, LOCAL};

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

void check_identifier(item);
void validar_funcao(int);
myvector new_vector(simbolo*, int);
void print_table();
void push_back(item);
void pop_back();
void pop_until_param();
item value_at(int);
void alter_at(int, item);
item *find_value(item);
int find_function_procudure();
int find_value_by_name(char*);
int find_value_by_name_local(char*);
int find_variable_by_name(char*);
int find_function_by_name(char*);
int find_procedure_by_name(char*);
int find_no_param_by_name(char*);
int find_until_function_by_name(char*);
int find_until_inside_function_by_name(char*);
bool cmp_item(item, item);
void free_item(item);
item new_item(char*, int, int, int);

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

int get_line();
int get_column();

#endif