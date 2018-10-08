#include "analex.h"
#define T_MAX 200

void append(char *s, char c){
    int tam = strlen(s);
    s[tam] = c;
    s[tam + 1] = '\0';
}

bool is_reserved(char* s){
    return false;
}

token new_token(categoria cat, char* s){
    token t;
    t.cat = cat;
    if(cat == IDENTIFIER || cat == RESERVED){
        t.value = malloc(strlen(s)+1);
        strcpy((char *) t.value, s);
    }
    else if(cat == CT_INT){
        t.value = malloc(sizeof(int));
        *cint(t.value) = atoi(s);
    }
    else if(cat == CT_FLOAT){
        t.value = malloc(sizeof(float));
        *cfloat(t.value) = atof(s);
    }
    else if(cat == L_MAIOR){
        t.value = NULL;
    }
    else if(cat == L_MENOR){
        t.value = NULL;
    }
    else if(cat == L_IGUAL){
        t.value = NULL;
    }
    return t;
}

void print_token(token t){
    if(t.cat == IDENTIFIER){
        printf("Tipo: IDENTIFICADOR\n");
        printf("Value: %s\n\n", cchar(t.value));
    }
    else if(t.cat == RESERVED){
        printf("Tipo: RESERVADO\n");
        printf("Value: %s\n\n", cchar(t.value));
    }
    else if(t.cat == CT_INT){
        printf("Tipo: INTEIRO\n");
        printf("Value: %i\n\n", *cint(t.value));
    }
    else if(t.cat == CT_FLOAT){
        printf("Tipo: PONTO FLUTUANTE\n");
        printf("Value: %f\n\n", *cfloat(t.value));
    }
    else if(t.cat == L_MAIOR){
        printf("Tipo: LOGICO\n");
        printf("Value: >\n\n");
    }
    else if(t.cat == L_MENOR){
        printf("Tipo: LOGICO\n");
        printf("Value: <\n\n");
    }
    else if(t.cat == L_IGUAL){
        printf("Tipo: LOGICO\n");
        printf("Value: =\n\n");
    }
}

token analise(FILE *entrada){
    int estado = 0;
    static int linha = 1, coluna = 1;
    char ax[T_MAX];
    memset(ax, 0, T_MAX);
    while(true){
        char c;
        switch(estado){
            case 0:
                c = getc(entrada);
                //coluna++;
                if(isalpha(c)){
                    estado = 1;
                }
                else if(isdigit(c)){
                    estado = 3;
                }
                else if(c == '='){
                    estado = 7;
                }
                else if(c == '>'){
                    estado = 10;
                }
                else if(c == '<'){
                    estado = 9;
                }
                else if(c == '\n'){
                    linha++;
                    coluna = 1;
                    break;
                }
                else if(c == ' '){
                    coluna++;
                    break;
                }
                else{
                    printf("Erro lexico na linha %i: coluna %i\n", linha, coluna);
                    exit(1);
                }
                append(ax, c);
                break;
            case 1:
                c = getc(entrada);
                coluna++;
                if(!(isalpha(c) || isdigit(c))) estado = 2;
                append(ax, c);
                break;
            case 2:
                ungetc(c, entrada);
                if(is_reserved(ax)) return new_token(IDENTIFIER, ax);
                else return new_token(RESERVED, ax);
            case 3:
                c = getc(entrada);
                coluna++;
                if(c == '.') estado = 5;
                else if(!isdigit(c)) estado = 4;
                append(ax, c);
                break;
            case 4:
                ungetc(c, entrada);
                return new_token(CT_INT, ax);
            case 5:
                c = getc(entrada);
                coluna++;
                if(!isdigit(c)) estado = 6;
                append(ax, c);
                break;
            case 6:
                ungetc(c, entrada);
                return new_token(CT_FLOAT, ax);
            case 7:
                c = getc(entrada);
                coluna++;
                estado = 8;
                append(ax, c);
            break;
            case 8:
                ungetc(c, entrada);
                return new_token(L_IGUAL, ax);
            case 9:
                c = getc(entrada);
                coluna++;
                estado = 12;
                append(ax, c);
            break;
            case 10:
                c = getc(entrada);
                coluna++;
                estado = 11;
                append(ax, c);
            break;
            case 11:
                ungetc(c, entrada);
                return new_token(L_MAIOR, ax);
            break;
            case 12:
                ungetc(c, entrada);
                return new_token(L_MENOR, ax);
            break;
            default:
                printf("Erro lexico na linha %i: coluna %i\n", linha, coluna);
                exit(1);
        }
    }
}