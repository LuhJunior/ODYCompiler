#ifndef SINTATICO_H
#define SINTATICO_H

#include "lexico.h"
#include "vector.h"
#include "tabela_simbolos.h"

enum simcat{ VARIAVEL, FUNCTION, PROCEDURE, FUNCTION_SIGNATURE, PROCEDURE_SIGNATURE, PARAM };

enum expression_operation{ S_MAIS, S_MENOS, S_VEZES, S_DIVISAO, S_AND, S_OR,
    S_MAIOR, S_MENOR, S_IGUAL, S_MENORIGUAL, S_MAIORIGUAL, S_HASHTAG };

enum escopo{ GLOBAL, LOCAL};

typedef struct{
    char type;
    void *value;
}Expression;

myvector *get_table();

void check_identifier(symbol*);
void validar_funcao(int);

bool attribution();

Expression new_expression(char, void*);
Expression comp_expression(Expression, char, Expression);
Expression expr();
Expression simple_expr();
Expression term();
Expression factor();
bool op_rel();
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

char const_type(char);

bool get_next();
void analise();
int get_line();
int get_column();

#endif