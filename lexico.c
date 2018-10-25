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

void ficarbig(char *s){
    for(int i=0; i<strlen(s); i++) s[i] = toupper(s[i]);
}

int binarySearch(char palavras[][RWTAM], char *palavra, int first, int last){
    if(strcmp(palavras[(first + last) / 2], palavra) < 0){
        if(first == last) return -1;
        return binarySearch(palavras, palavra, first, (first + last) / 2);
    }
    else if(strcmp(palavras[(first + last) / 2], palavra) == 0){
        return (first + last) / 2;
    }
    else if(strcmp(palavras[(first + last) / 2], palavra) > 1){
        if(first == last) return -1;
        return binarySearch(palavras, palavra, (first + last) / 2, last);
    }
    return -1;
}

int is_reserved(char* s){
    char palavra[50];
    strcpy(palavra, s);
    ficarbig(palavra);
    return binarySearch(reserved_words, palavra, 0, RWTAM-1);
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
        *cchar(t.value) = *cchar(valor);
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
        *cchar(t.value) = *cchar(valor);
    }
    else if(cat == OPERADOR){
        t.value = malloc(1);
        *cchar(t.value) = *cchar(valor);
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
        printf("Value: %s\n\n", reserved_words[*cint(t.value)]);
    }
    else if(t.cat == CT_INT){
        printf("Tipo: INTEIRO\n");
        printf("Value: %i\n\n", *cint(t.value));
    }
    else if(t.cat == CT_FLOAT){
        printf("Tipo: PONTO FLUTUANTE\n");
        printf("Value: %f\n\n", *cfloat(t.value));
    }
    else if(t.cat == LOGICO){
        printf("Tipo: LOGICO\n");
        switch(*cint(t.value)){
            case 0:
                printf("Value: >\n\n");
                break;
            case 1:
                printf("Value: <\n\n");
                break;
            case 2:
                printf("Value: =\n\n");
                break;
            case 3:
                printf("Value: <=\n\n");
                break;
            case 4:
                printf("Value: >=\n\n");
                break;
            case 5:
                printf("Value: .not.\n\n");
                break;
            case 6:
                printf("Value: .and.\n\n");
                break;
            case 7:
                printf("Value: .or.\n\n");
                break;
        }
    }
}

token analise(FILE *entrada){
    int estado = 0;
    char valor = -1;
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
                else if(c == '.'){
                    estado = 36;
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
                else if(c == '\''){
                    estado = 15;
                }
                else if(c == '\"'){
                    estado = 23;
                }
                else if(c == '\\'){
                    estado = 27;
                }
                else if(c == '+'){
                    estado = 31;
                }
                else if(c == '-'){
                    estado = 33;
                }
                else if(c == '*'){
                    estado = 35;
                }
                else if(c == '/'){
                    estado = 37;
                }
                else if(c == '#'){
                    estado = 25;
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
                //FINAL
                ungetc(c, entrada);
                int palavra = is_reserved(ax);
                printf("%i\n", palavra);
                if(palavra == -1) return new_token(IDENTIFIER, ax);
                else return new_token(RESERVED, &palavra);
            case 3:
                c = getc(entrada);
                coluna++;
                if(c == '.') estado = 5;
                else if(!isdigit(c)) estado = 4;
                append(ax, c);
                break;
            case 4:
                //FINAL
                ungetc(c, entrada);
                return new_token(CT_INT, ax);
            case 5:
                c = getc(entrada);
                coluna++;
                if(isdigit(c)) estado = 38;
                else if(c == 'a' || c == 'o' || c == 'n') estado = 14;
                else estado = -1;
                append(ax, c);
                break;
            case 6:
                //FINAL
                ungetc(c, entrada);
                return new_token(CT_FLOAT, ax);
            case 7:
                c = getc(entrada);
                coluna++;
                if(c == '=') estado = 18;
                else estado = 8;
                append(ax, c);
                break;
            case 8:
                //FINAL  
                ungetc(c, entrada);
                valor = ATRIBUICAO;
                return new_token(OPERADOR, &valor);
            case 9:
                c = getc(entrada);
                coluna++;
                if(c == '=') estado = 16;
                else estado = 12;
                append(ax, c);
                break;
            case 10:
                c = getc(entrada);
                coluna++;
                if(c == '=') estado = 13;
                else estado = 11;
                append(ax, c);
                break;
            case 11:
                //FINAL
                ungetc(c, entrada);
                valor = MAIOR;
                return new_token(LOGICO, &valor);
            case 12:
                //FINAL
                ungetc(c, entrada);
                valor = MENOR;
                return new_token(LOGICO, &valor);
                break;
            case 13:
                //FINAL
                valor = MAIORIGUAL;
                return new_token(LOGICO, &valor);
            case 14:
                ungetc(c, entrada);
                ungetc(ax[strlen(ax) - 1], entrada);
                ax[strlen(ax) - 1] = '\0';
                return new_token(CT_FLOAT, ax);
            case 15:
                c = getc(entrada);
                coluna++;
                if(isalpha(c) || c == '\r' || c == 0) estado = 20;
                append(ax, c);
                break;
            case 16:
                //FINAL
                valor = MENORIGUAL;
                return new_token(LOGICO, &valor);
            case 17:
                c = getc(entrada);
                coluna++;
                if(c == '.') estado = 22;
                else estado = -1;
                break;
            case 18:
                valor = IGUAL;
                return new_token(LOGICO, &valor);
            case 19:
                c = getc(entrada);
                coluna++;
                if(c == '.') estado = 32;
                else estado = -1;
                break;
            case 20:
                c = getc(entrada);
                coluna++;
                if(c == '\'') estado = 32;
                else estado = -1;
                append(ax, c);
                break;
            case 21:
                return new_token(CT_CARACTER, ax);
            case 22:
                valor = AND;
                return new_token(LOGICO, &valor);
            case 23:
                c = getc(entrada);
                coluna++;
                if(c == '\"') estado = 24;
                //else if(c == '\r') ...
                append(ax, c);
                break;
            case 24:
                return new_token(CT_STRING, ax);
            case 25:
                valor = HASHTAG;
                return new_token(OPERADOR, &valor);
            case 26:
                c = getc(entrada);
                coluna++;
                if(c == '/') estado = 27;
                else estado = 29;
                break;
            case 27:
                c = getc(entrada);
                coluna++;
                if(c == '\n') estado = 0;
                break;
            case 28:
                c = getc(entrada);
                coluna++;
                if(c == 'd') estado = 17;
                else if(c != 'n') estado = -1;
                break;
            case 29:
                ungetc(c, entrada);
                valor = DIVISAO;
                return new_token(OPERADOR, &valor);
            case 30:
                c = getc(entrada);
                coluna++;
                if(c == '.') estado = 34;
                else estado = -1;
                break;
            case 31:
                valor = MAIS;
                return new_token(OPERADOR, &valor);
            case 32:
                valor = OR;
                return new_token(LOGICO, &valor);
            case 33:
                valor = MENOS;
                return new_token(OPERADOR, &valor);
            case 34:
                valor = NOT;
                return new_token(LOGICO, &valor);
            case 35:
                valor = VEZES;
                return new_token(OPERADOR, &valor);
            case 36:
                c = getc(entrada);
                coluna++;
                if(c == 'a') estado = 28;
                else if(c == 'o') estado = 37;
                else if(c == 'n' ) estado = 39;
                break;
            case 37:
                c = getc(entrada);
                coluna++;
                if(c == 'r') estado = 19;
                else estado = -1;
                break;
            case 38:
                c = getc(entrada);
                coluna++;
                if(!isdigit(c)) estado = 6;
                else append(ax, c);
                break;
            case 39:
                c = getc(entrada);
                coluna++;
                if(c == 't') estado = 30;
                else if(c != 'o') estado = -1;
                break;
            case 40:
            
                break;
            case 41:
            
                break;
            case 42:
            
                break;
            case 43:
            
                break;
            case 44:
            
                break;
            default:
                printf("Erro lexico na linha %i: coluna %i\n", linha, coluna);
                exit(1);
        }
    }
}