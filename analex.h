#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define cint(x) (int *) x
#define cfloat(x) (float *) x
#define cchar(x) (char *) x
#define true 1
#define false 0

typedef char bool;

typedef enum { IDENTIFIER, RESERVED, L_MAIOR, L_MENOR, L_IGUAL, L_MENORIGUAL, L_MAIORIGUAL,
                CT_INT, CT_FLOAT, CT_CARACTER, CT_STRING } categoria;

typedef struct token{
    categoria cat;
    void *value;
} token;

bool is_reserved(char *);

void append(char *, char);

token analise(FILE *);

token new_token(categoria, char*);

void print_token(token);