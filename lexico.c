#include "lexico.h"
#define T_MAX 200

FILE *ENTRADA;
int linha = 1, coluna = 1;

bool prepare_file(char *s){
    ENTRADA = fopen(s, "r");
    return ENTRADA != NULL;
}

bool close_file(){
    return fclose(ENTRADA) == 0;
}

int get_linha(){
    return linha;
}

int get_coluna(){
    return coluna;
}

char get_next_char(){
    char c = getc(ENTRADA);
    if(c == EOF){
        printf("Erro: Fim do arquivo antes da validacao do Token\n");
        exit(0);
    }
    return c;
}

void unget_char(char c){
    ungetc(c, ENTRADA);
    coluna--;
}

char reserved_words[][RWTAM] = { "bool", "call", "char", "display", "dup", "else", "endfor", "endif", "endfunc", "endproc", 
                                "endprog", "endvar", "endwhile", "for", "fwd", "id", "if", "int", "keyboard", "noparam", 
                                "pl", "proc", "prog", "real", "return", "var", "while" };

void append(char *s, char c){
    int tam = strlen(s);
    s[tam] = c;
    s[tam + 1] = '\0';
}

char unappend(char *s){
    int tam = strlen(s);
    char c = s[tam - 1];
    if(tam > 0) s[tam - 1] = '\0';
    return c;
}

void ficarbig(char *s){
    for(int i=0; i<strlen(s); i++) s[i] = toupper(s[i]);
}

int binarySearch(char palavras[][RWTAM], char *palavra, int first, int last){
    if(strcmp(palavra, palavras[(first + last) / 2]) < 0){
        if(first >= last) return -1;
        return binarySearch(palavras, palavra, first, ((first + last) / 2) - 1);
    }
    else if(strcmp(palavra, palavras[(first + last) / 2]) == 0){
        return (first + last) / 2;
    }
    else{
        if(first >= last) return -1;
        return binarySearch(palavras, palavra, ((first + last) / 2) + 1, last);
    }
}

int is_reserved(char* s){
    //char palavra[50];
    //strcpy(palavra, s);
    return binarySearch(reserved_words, s, 0, RWTAM-1);
}

token new_token(categoria cat, void* valor){
    token t;
    t.cat = cat;
    t.value = NULL;
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
    else if(cat == CT_CARACTER){
        t.value = malloc(1);
        *cchar(t.value) = *cchar(valor);
    }
    else if(cat == CT_STRING){
        t.value = malloc(strlen(cchar(valor))+1);
        strcpy((char *) t.value, cchar(valor));
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
        printf("<IDENTIFICADOR, ");
        printf("%s >\n\n", cchar(t.value));
    }
    else if(t.cat == RESERVED){
        printf("<RESERVADO, ");
        printf("%s >\n\n", reserved_words[*cchar(t.value)]);
    }
    else if(t.cat == CT_INT){
        printf("<INTEIRO, ");
        printf("%i >\n\n", *cint(t.value));
    }
    else if(t.cat == CT_FLOAT){
        printf("<PONTO FLUTUANTE, ");
        printf("%f >\n\n", *cfloat(t.value));
    }
    else if(t.cat == CT_CARACTER){
        printf("<CARACTER, ");
        if(*cchar(t.value) >= 32 && *cchar(t.value) <= 126) printf("\'%c\' >\n\n", *cchar(t.value));
        else if(*cchar(t.value) == 0) printf("NULL >\n\n");
        else if(*cchar(t.value) == '\r') printf("Carriage Return >\n\n");
        else if(*cchar(t.value) == '\n') printf("New Line >\n\n");
    }
    else if(t.cat == CT_STRING){
        printf("<STRING, ");
        printf("\"%s\" >\n\n", cchar(t.value));
    }
    else if(t.cat == LOGICO){
        printf("<LOGICO, ");
        switch(*cchar(t.value)){
            case 0:
                printf("\">\" >\n\n");
                break;
            case 1:
                printf("\"<\" >\n\n");
                break;
            case 2:
                printf("\"==\" >\n\n");
                break;
            case 3:
                printf("\"<=\" >\n\n");
                break;
            case 4:
                printf("\">=\">\n\n");
                break;
            case 5:
                printf("\"#\" >\n\n");
                break;
            case 6:
                printf("not >\n\n");
                break;
            case 7:
                printf("and >\n\n");
                break;
            case 8:
                printf("or >\n\n");
                break;
        }
    }
    else if(t.cat == OPERADOR){
        printf("<OPERADOR, ");
        switch(*cchar(t.value)){
            case 0:
                printf("\"=\" >\n\n");
                break;
            case 1:
                printf("\"+\" >\n\n");
                break;
            case 2:
                printf("\"-\" >\n\n");
                break;
            case 3:
                printf("\"*\" >\n\n");
                break;
            case 4:
                printf("\"/\" >\n\n");
                break;
            case 5:
                printf("\"(\" >\n\n");
                break;
            case 6:
                printf("\")\" >\n\n");
                break;
            case 7:
                printf("\",\" >\n\n");
                break;
            case 8:
                printf("\";\" >\n\n");
                break;
            case 9:
                printf("\"[\" >\n\n");
                break;
            case 10:
                printf("\"]\" >\n\n");
                break;
        }
    }
}

void free_token(token t){
    switch(t.cat){
        case IDENTIFIER:
            free(cchar(t.value));
            break;
        case RESERVED:
            free(cchar(t.value));
            break;
        case CT_INT:
            free(cint(t.value));
            break;
        case CT_FLOAT:
            free(cfloat(t.value));
            break;
        case CT_CARACTER:
            free(cchar(t.value));
            break;
        case CT_STRING:
            free(cchar(t.value));
            break;
        case LOGICO:
            free(cchar(t.value));
            break;
        case OPERADOR:
            free(cchar(t.value));
        break;
    }
}

token get_token(){
    int estado = 0;
    char valor = -1;
    char ax[T_MAX];
    memset(ax, 0, T_MAX);
    while(true){
        char c;
        switch(estado){
            case 0:
                c = get_next_char();
                coluna++;
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
                    continue;
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
                    estado = 26;
                }
                else if(c == '#'){
                    estado = 25;
                }
                else if(c == '('){
                    estado = 40;
                }
                else if(c == ')'){
                    estado = 43;
                }
                else if(c == '['){
                    estado = 44;
                }
                else if(c == ']'){
                    estado = 45;
                }
                else if(c == ','){
                    estado = 46;
                }
                else if(c == ';'){
                    estado = 47;
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
                else if(c == -1){
                    return new_token(FIM_ARQUIVO, NULL);
                }
                else{
                    printf("Erro lexico na linha %i: coluna %i\n", linha, coluna);
                    exit(1);
                }
                append(ax, c);
                break;
            case 1:
                c = get_next_char();
                coluna++;
                if(!(isalpha(c) || isdigit(c))) estado = 2;
                /* Na especificação não tem dizendo o tamanho máximo do identificador
                if(strlen(ax) > 10){
                    estado = -1;
                    printf("Um identificador nao pode ter mais que 10 caracteres\n");
                }
                */
                append(ax, c);
                break;
            case 2:
                //FINAL
                unget_char(unappend(ax));
                char palavra = is_reserved(ax);
                if(palavra == -1) return new_token(IDENTIFIER, ax);
                else return new_token(RESERVED, &palavra);
            case 3:
                c = get_next_char();
                coluna++;
                if(c == '.') estado = 5;
                else if(!isdigit(c)) estado = 4;
                append(ax, c);
                break;
            case 4:
                //FINAL
                unget_char(unappend(ax));
                return new_token(CT_INT, ax);
            case 5:
                c = get_next_char();
                coluna++;
                if(isdigit(c)) estado = 38;
                else if(c == 'a' || c == 'o' || c == 'n') estado = 14;
                else estado = -1;
                append(ax, c);
                break;
            case 6:
                //FINAL
                unget_char(c);
                return new_token(CT_FLOAT, ax);
            case 7:
                c = get_next_char();
                coluna++;
                if(c == '=') estado = 18;
                else estado = 8;
                append(ax, c);
                break;
            case 8:
                //FINAL  
                unget_char(c);
                valor = ATRIBUICAO;
                return new_token(OPERADOR, &valor);
            case 9:
                c = get_next_char();
                coluna++;
                if(c == '=') estado = 16;
                else estado = 12;
                append(ax, c);
                break;
            case 10:
                c = get_next_char();
                coluna++;
                if(c == '=') estado = 13;
                else estado = 11;
                append(ax, c);
                break;
            case 11:
                //FINAL
                unget_char(c);
                valor = MAIOR;
                return new_token(LOGICO, &valor);
            case 12:
                //FINAL
                unget_char(c);
                valor = MENOR;
                return new_token(LOGICO, &valor);
                break;
            case 13:
                //FINAL
                valor = MAIORIGUAL;
                return new_token(LOGICO, &valor);
            case 14:
                unget_char(unappend(ax));
                unget_char(unappend(ax));
                return new_token(CT_INT, ax);
            case 15:
                c = get_next_char();
                coluna++;
                //caracteres imprimiveis esta no range [32 ; 126]
                if((c >= 32 && c <= 126) || c == '\n' || c == '\r' || c == 0){
                    if(c == '\\'){
                        c = get_next_char();
                        if(c == 'r'){
                            append(ax, '\r');
                            estado = 20;  
                        }
                        else if(c == 'n'){
                            append(ax, '\n');
                            estado = 20;
                        }
                        else if(c == '\t'){
                            coluna += 3;
                        }
                        else{
                            unget_char(c);
                            append(ax, '\\');
                            estado = 20;
                        }
                    }
                    else{
                        append(ax, c);
                        estado = 20;
                    }
                }
                else estado = -1;
                break;
            case 16:
                //FINAL
                valor = MENORIGUAL;
                return new_token(LOGICO, &valor);
            case 17:
                c = get_next_char();
                coluna++;
                if(c == '.') estado = 22;
                else estado = -1;
                break;
            case 18:
                valor = IGUAL;
                return new_token(LOGICO, &valor);
            case 19:
                c = get_next_char();
                coluna++;
                if(c == '.') estado = 32;
                else estado = -1;
                break;
            case 20:
                c = get_next_char();
                coluna++;
                if(c == '\'') estado = 21;
                else estado = -1;
                append(ax, c);
                break;
            case 21:
                valor = ax[1];
                return new_token(CT_CARACTER, &valor);
            case 22:
                valor = AND;
                return new_token(LOGICO, &valor);
            case 23:
                c = get_next_char();
                coluna++;
                if(c == '\\'){
                    c = get_next_char();
                    if(c == 'r') estado = -1;
                    else{
                        unget_char(c);
                        c = '\\';
                    }
                }
                if(c == '\"') estado = 24;
                append(ax, c);
                break;
            case 24:
                unappend(ax);
                return new_token(CT_STRING, (void *) ax);
            case 25:
                valor = HASHTAG;
                return new_token(OPERADOR, &valor);
            case 26:
                c = get_next_char();
                coluna++;
                if(c == '/'){
                    coluna = 1;
                    estado = 27;
                    unappend(ax);
                }
                else estado = 29;
                break;
            case 27:
                c = get_next_char();
                coluna++;
                if(c == '\n') estado = 0;
                break;
            case 28:
                c = get_next_char();
                coluna++;
                if(c == 'n') estado = 41;
                else estado = -1;
                break;
            case 29:
                unget_char(c);
                valor = DIVISAO;
                return new_token(OPERADOR, &valor);
            case 30:
                c = get_next_char();
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
                c = get_next_char();
                coluna++;
                if(c == 'a') estado = 28;
                else if(c == 'o') estado = 37;
                else if(c == 'n' ) estado = 39;
                break;
            case 37:
                c = get_next_char();
                coluna++;
                if(c == 'r') estado = 19;
                else estado = -1;
                break;
            case 38:
                c = get_next_char();
                coluna++;
                if(!isdigit(c)) estado = 6;
                else append(ax, c);
                break;
            case 39:
                c = get_next_char();
                coluna++;
                if(c == 'o') estado = 42;
                else estado = -1;
                break;
            case 40:
                valor = ABRE_PARENTESE;
                return new_token(OPERADOR, &valor);
            case 41:
                c = get_next_char();
                coluna++;
                if(c == 'd') estado = 17;
                else estado = -1;
                break;
            case 42:
                c = get_next_char();
                coluna++;
                if(c == 't') estado = 30;
                else estado = -1;
                break;
            case 43:
                valor = FECHA_PARENTESE;
                return new_token(OPERADOR, &valor);
            case 44:
                valor = ABRE_COLCHETE;
                return new_token(OPERADOR, &valor);
            case 45:
                valor = FECHA_COLCHETE;
                return new_token(OPERADOR, &valor);
            case 46:
                valor = VIRGULA;
                return new_token(OPERADOR, &valor);
            case 47:
                valor = PONTO_VIRGULA;
                return new_token(OPERADOR, &valor);
            default:
                printf("Erro lexico na linha %i: coluna %i\n", linha, coluna);
                exit(1);
        }
    }
}

bool has_next(token t){
    return t.cat != FIM_ARQUIVO;
}