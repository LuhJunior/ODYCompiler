#ifndef LEXICO_H
#define LEXICO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define RWTAM 24
#define cint(x) (int *) x
#define cfloat(x) (float *) x
#define cchar(x) (char *) x
#define true 1
#define false 0

typedef char bool;

typedef enum { IDENTIFIER, RESERVED, LOGICO, OPERADOR, CT_INT, CT_FLOAT, CT_CARACTER, CT_STRING, FIM_ARQUIVO } categoria;

typedef enum { MAIOR, MENOR, IGUAL, MENORIGUAL, MAIORIGUAL, NOT, AND, OR } tipo_logico;

typedef enum { ATRIBUICAO, MAIS, MENOS, VEZES, DIVISAO, HASHTAG, ABRE_PARENTESE, 
                FECHA_PARENTESE, VIRGULA, PONTO_VIRGULA, ABRE_COLCHETE, FECHA_COLCHETE} operadores;

typedef enum { BOOL, CALL, CHAR, DISPLAY, ELSE, ENDFOR, ENDIF, ENDPROC, ENDPROG, ENDVAR, ENDWHILE,
                FOR, FWD, ID, IF, INT, NOPARAM, PL, PROC, PROG, REAL, RETURN, VAR, WHILE } r_words;

typedef struct token{
    categoria cat;
    void *value;
} token;

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
int is_reserved(char *);

int binarySearch(char [][RWTAM], char *, int, int);

void append(char *, char);

char unappend(char *);

token get_token();

token new_token(categoria, void*);

void free_token(token);

void print_token(token);

bool has_next(token);

void ficarbig(char *);

char get_next_char();

void unget_char(char);

bool prepare_file(char *);

bool close_file();

#endif