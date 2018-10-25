#include "lexico.h"
#define T_MAX 200

char reserved_words[][RWTAM] = { "BOOL", "CALL", "CHAR", "DISPLAY", "ELSE", "ENDFOR", "ENDIF", "ENDPROC", 
                            "ENDPROG", "ENDVAR", "ENDWHILE", "FOR", "FWD", "ID", "IF", "INT", "NOPARAM", 
                            "PL", "PROC", "PROG", "REAL", "RETURN", "VAR", "WHILE" };

void append(char *s, char c){
    int tam = strlen(s);
    s[tam] = c;
    s[tam + 1] = '\0';
}

int binarySearch(char palavras[][RWTAM], char *palavra, int first, int last){
    if(strcmp(palavras[(first + last) / 2], palavra) < 0){
        if(first == last) return -1;
        return binarySearch(palavras, palavra, first, (first + last) / 2);
    }
    else if(strcmp(palavras[(first + last) / 2], palavra) == 0){
        if(first == last) return -1;
        return (first + last) / 2;
    }
    else if(strcmp(palavras[(first + last) / 2], palavra) > 1){
        if(first == last) return -1;
        return binarySearch(palavras, palavra, (first + last) / 2, last);
    }
    return -1;
}

int is_reserved(char* s){
    return binarySearch(reserved_words, s, 0, RWTAM-1);
}

token new_token(categoria cat, void* valor){
    token t;
    t.cat = cat;
    if(cat == IDENTIFIER){
        t.value = malloc(strlen(cchar(valor))+1);
        strcpy((char *) t.value, cchar(valor));
    }
    else if(cat == RESERVED){
        t.value = malloc(1);
        *cchar(t.value) = cint(valor);
    }
    else if(cat == CT_INT){
        t.value = malloc(sizeof(int));
        *cint(t.value) = atoi(cchar(valor));
    }
    else if(cat == CT_FLOAT){
        t.value = malloc(sizeof(float));
        *cfloat(t.value) = atof(cchar(valor));
    }
    else if(cat == LOGICO){
        t.value = malloc(1);
        *cchar(t.value) = cint(valor);
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
                int palavra = is_reserved(ax);
                if(palavra == -1) return new_token(RESERVED, &palavra);
                else return new_token(IDENTIFIER, ax);
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