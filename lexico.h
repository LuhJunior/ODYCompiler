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

typedef enum { IDENTIFIER, RESERVED, LOGICO, OPERADOR, CT_INT, CT_FLOAT, CT_CARACTER, CT_STRING } categoria;

typedef enum { MAIOR, MENOR, IGUAL, MENORIGUAL, MAIORIGUAL, NOT, AND, OR } tipo_logico;

typedef enum { ATRIBUICAO, MAIS, MENOS, VEZES, DIVISAO, HASHTAG} operadores;

typedef enum { BOOL, CALL, CHAR, DISPLAY, ELSE, ENDFOR, ENDIF, ENDPROC, ENDPROG, ENDVAR, ENDWHILE,
                FOR, FWD, ID, IF, INT, NOPARAM, PL, PROC, PROG, REAL, RETURN, VAR, WHILE } r_words;

typedef struct token{
    categoria cat;
    void *value;
} token;

int is_reserved(char *);

int binarySearch(char [][RWTAM], char *, int, int);

void append(char *, char);

token analise(FILE *);

token new_token(categoria, void*);

void print_token(token);

void ficarbig(char *);

#endif