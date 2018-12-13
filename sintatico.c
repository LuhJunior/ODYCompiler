#include "sintatico.h"
#include "error.h"

token t, t_ahead;
int line, column;
myvector table;
char expression_type;
char inside_function = 0;

// FUNCOES DO MYVECTOR

bool cmp_item(item x, item y){
    return (strcmp(x.valor->nome, y.valor->nome) && x.valor->scopo == y.valor->scopo && 
        x.valor->categoria == y.valor->categoria && x.valor->tipo == y.valor->tipo);
}

void free_item(item i){
    if(i.valor->nome) free(i.valor->nome);
    if(i.valor) free(i.valor);
}

item new_item(char* nome, int categoria, int tipo, int scopo){
    item i;
    i.valor = (simbolo *) malloc(sizeof(simbolo));
    if(!i.valor) execution_erro(1);
    i.valor->nome = NULL;
    if(nome){
        i.valor->nome = (char*) malloc(strlen(nome) + 1);
        if(!i.valor->nome) execution_erro(1);
        strcpy(i.valor->nome, nome);
    }
    i.valor->categoria = categoria;
    i.valor->tipo = tipo;
    i.valor->scopo = scopo;
    return i;
}

myvector new_vector(simbolo* value, int tam){
    myvector v;
    v.value = (item *) malloc(sizeof(item) * tam + 10);
    if(!v.value) execution_erro(1);
    v.tam = tam + 10;
    v.current = -1;
    for(int i=0; i<tam && value != NULL; i++) v.value[i].valor = value;
    return v;
}

void free_vector(){
    for(int i=table.current; i>=0; i--) free_item(table.value[i]);
    if(table.value) free(table.value);
}

void push_back(item value){
    if(table.current == table.tam){
        table.tam += 10;
        item *values, *valor;
        values = valor = (item *) malloc(sizeof(item) * table.tam);
        memcpy(values, table.value, sizeof(item) * table.tam);
        free(table.value);
        table.value = values;
        //realloc(table.value, sizeof(item) * table.tam);
    }
    table.value[++table.current].valor = value.valor;
}

void pop_back(){
    if(table.current >= 0) free_item(table.value[table.current--]);
}

void pop_until_param(){
    for(int i = inside_function; i<=table.current; i++) 
        if(table.value[i].valor->categoria == VARIAVEL && table.value[i].valor->scopo == LOCAL) pop_back();
        else if(table.value[i].valor->categoria == PARAM) break;
}

item value_at(int p){
    if(p <= table.current && p>=0) return(table.value[p]);
    printf("out of range: %i", p);
    return new_item(NULL, -1, -1, -1);
}

void alter_at(int p, item i){
    free_item(table.value[p]);
    table.value[p] = i;
}

item *find_value(item value){
    for(int i=0; i<table.current; i++) if(cmp_item(table.value[i], value)) return table.value + i;
    return NULL;
}

int find_value_by_name(char* s){
    if(inside_function){
        int position = find_until_inside_function_by_name(s);
        if(position != -1) return position;
    }
    return find_no_param_by_name(s);
}

int find_no_param_by_name(char* s){
    for(int i=table.current; i>=0; i--){ if(table.value[i].valor->nome && strcmp(table.value[i].valor->nome, s) == 0 && 
        table.value[i].valor->categoria != PARAM) return i;
    }
    return -1;
}

int find_function_procudure(){
    for(int i=table.current; i>=0; i--)
        if(table.value[i].valor->categoria == FUNCTION || 
        table.value[i].valor->categoria == FUNCTION_SIGNATURE ||
        table.value[i].valor->categoria == PROCEDURE ||
        table.value[i].valor->categoria == PROCEDURE_SIGNATURE) return i;
    return -1;
}

int find_function_by_name(char* s){
    for(int i=table.current; i>=0; i--)
        if(table.value[i].valor->nome && strcmp(table.value[i].valor->nome, s) == 0){
            if(table.value[i].valor->categoria == FUNCTION || table.value[i].valor->categoria == FUNCTION_SIGNATURE) return i;
            break;
        }
    return -1;
}

int find_procedure_by_name(char* s){
    for(int i=table.current; i>=0; i--)
        if(table.value[i].valor->nome && strcmp(table.value[i].valor->nome, s) == 0){
            if(table.value[i].valor->categoria == PROCEDURE || table.value[i].valor->categoria == PROCEDURE_SIGNATURE) return i;
            break;
        }
    return -1;
}

int find_variable_by_name(char* s){
    for(int i=table.current; i>=0; i--) 
        if(table.value[i].valor->nome && strcmp(table.value[i].valor->nome, s) == 0){
            if(table.value[i].valor->categoria == VARIAVEL) return i;
            break;
        }
    return -1;
}

int find_value_by_name_local(char* s){
    for(int i=inside_function+1; i<=table.current && table.value[i].valor->scopo == LOCAL; i++)
        if(table.value[i].valor->nome)
            if(strcmp(table.value[i].valor->nome, s) == 0) return i;
    return -1;
}

int find_until_function_by_name(char* s){
    for(int i=table.current; i>=0 && table.value[i].valor->categoria != FUNCTION &&
        table.value[i].valor->categoria != PROCEDURE && 
        table.value[i].valor->categoria != FUNCTION_SIGNATURE &&
        table.value[i].valor->categoria != PROCEDURE_SIGNATURE; i--)
        if(table.value[i].valor->nome && strcmp(table.value[i].valor->nome, s) == 0) return i;
    return -1;
}

int find_until_inside_function_by_name(char* s){
    for(int i=table.current; i>inside_function; i--)
        if(table.value[i].valor->nome && strcmp(table.value[i].valor->nome, s) == 0) return i;
    return -1;
}

void print_table(){
    for(int i=0; i<=table.current; i++) 
        printf("| Nome : %s | Tipo: %i | Categoria: %i | Scopo: %i|\n", 
            table.value[i].valor->nome ? table.value[i].valor->nome : "",
            table.value[i].valor->tipo, table.value[i].valor->categoria,
            table.value[i].valor->scopo);
}


// GET LINHA E COLUNA

int get_line(){
    return line;
}

int get_column(){
    return column;
}

// FUNCOES DO TOKEN

// CATEGORIA E VALOR

char operator(){
    if(t.cat != OPERADOR) return -1;
    return *cchar(t.value);
}

char reserved(){
    if(t.cat != RESERVED) return -1;
    return *cchar(t.value);
}

// PEGAR VALORES DO TOKEN

char t_char(){
    return *cchar(t.value);
}

char t2_char(){
    return *cchar(t_ahead.value);
}

int t_int(){
    return *cint(t.value);
}

float t_float(){
    return *cfloat(t.value);
}

char *t_string(){
    return cchar(t.value);
}

// PEGAR TOKEN

bool get_first_token(){
    t = get_token();
    t_ahead = get_token();
}

bool get_next(){
    if(!has_next(t)){
        free_token(t);
        free_token(t_ahead);
        return false;
    }
    free_token(t);
    t = t_ahead;
    line = get_linha();
    column = get_coluna();
    if(has_next(t_ahead)) t_ahead = get_token();
    print_token(t);
    return true;
}

// VERIFICACAO DO TIPO DO TOKEN

bool type(){
    return (t.cat == RESERVED && (t_char() == CHAR || t_char() == INT || t_char() == REAL
            || t_char() == BOOL));
}

bool type_ahead(){
    return (t_ahead.cat == RESERVED && (t2_char() == CHAR || t2_char() == INT || t2_char() == REAL
            || t2_char() == BOOL));
}

bool identifier(){
    return (t.cat == IDENTIFIER);
}

bool ct_int(){
    return (t.cat == CT_INT);
}

bool ct_car(){
    return (t.cat == CT_CARACTER);
}

bool ct_string(){
    return (t.cat == CT_STRING);
}

bool ct_real(){
    return (t.cat == CT_FLOAT);
}

char const_type(char cat){
    switch(cat){
        case CT_INT:
            return INT;
        case CT_FLOAT:
            return REAL;
        case CT_CARACTER:
            return CHAR;
        default:
            return -1;
    }
}

char expression_op(){
    if(t_ahead.cat == OPERADOR){
        switch(t2_char()){
            case MAIS: return S_MAIS;
            case MENOS: return S_MENOS;
            case VEZES: return S_VEZES;
            case DIVISAO: return S_DIVISAO;
        }
    }
    else if(t_ahead.cat == LOGICO){
        switch(t2_char()){
            case MAIOR: return S_MAIOR;
            case MENOR: return S_MENOR;
            case IGUAL: return S_IGUAL;
            case MENORIGUAL: return S_MENORIGUAL;
            case MAIORIGUAL: return S_MAIORIGUAL;
            case HASHTAG: return S_HASHTAG;
            case AND: return S_AND;
            case OR: return S_OR;
        }
    }
    return -1;
}

Expression new_expression(char type, void *value){
    Expression e;
    e.type = type;
    //e.value = value;
    e.value = NULL;
    return e;
}

//VERIFICACOES SEMANTICAS

void check_identifier(item ident){
    int position = -1;
    switch(ident.valor->categoria){
        case VARIAVEL:
            if(ident.valor->scopo == GLOBAL) position = find_no_param_by_name(ident.valor->nome);
            else position = find_value_by_name_local(ident.valor->nome);
            if(position != -1) error2(0);
            break;
        case FUNCTION_SIGNATURE:
            position = find_value_by_name(ident.valor->nome);
            if(position != -1) error2(0);
            break;
        case PROCEDURE_SIGNATURE:
            position = find_value_by_name(ident.valor->nome);
            if(position != -1) error2(0);
            break;
        case FUNCTION:
            position = find_value_by_name(ident.valor->nome);
            inside_function = position;
            if(position != -1){
                if(!(value_at(position).valor->categoria == FUNCTION_SIGNATURE)) error2(0);
                if(!(value_at(position).valor->tipo == ident.valor->tipo)) error2(1);
                param(position);
                alter_at(position, ident);
            }
            else{
                push_back(ident);
                inside_function = table.current;
                param(-1);
            }
            break;
        case PROCEDURE:
            position = find_value_by_name(ident.valor->nome);
            inside_function = position;
            if(position != -1){
                if(!(value_at(position).valor->categoria == PROCEDURE_SIGNATURE)) error2(0);
                param(position);
                alter_at(position, ident);
            }
            else{
                push_back(ident);
                inside_function = table.current;
                param(-1);
            }
            break;
        case PARAM:
            position = find_value_by_name_local(ident.valor->nome);
            if(position != -1) error2(0);
            break;
        default:
            error2(14);
            break;
    }
}

Expression comp_expression(Expression e, char op, Expression e2){
    /*Se e é uma expressão na forma e1 + e2, e1 - e2, e1 * e2, e1 / e2, ou -e1, então o
    tipo de e é compatível com os tipos dos elementos da expressão, retritos a int,
    char e real, ou seja, em “e1 + e2” se e1 for char e e2 for int, a operação é
    possível porque estes tipos possuem compatibilidade entre si e o tipo da
    expressão fica sendo int; por outro lado, se e1 for um int e e2 for um real um
    conflito de tipos é estabelecido e uma mensagem de erro deve ser emitida;
    Se e é uma expressão na forma e1 >= e2, e1 <= e2, e1 > e2, e1 < e2, e1 == e2, ou
    e1 # e2 então o tipo de e é bool.*/
    if((e.type == CHAR && e2.type == INT) || (e.type == INT && e2.type == CHAR) ||
        (e.type == INT && e2.type == BOOL) || (e.type == BOOL && e2.type == INT)){
        if(op == S_MAIS || op == S_MENOS || op == S_VEZES || op == S_DIVISAO) return new_expression(INT, NULL);
        else if(op == S_OR || op == S_AND || op == S_MAIOR || op == S_MENOR ||
            op == S_MAIORIGUAL || op == S_MENORIGUAL || op == S_IGUAL || op ==S_HASHTAG) return new_expression(BOOL, NULL);
    }
    else if(e.type == REAL && e2.type == REAL){
        if(op == S_MAIS || op == S_MENOS || op == S_VEZES || op == S_DIVISAO) return new_expression(REAL, NULL);
        else if(op == S_OR || op == S_AND || op == S_MAIOR || op == S_MENOR ||
            op == S_MAIORIGUAL || op == S_MENORIGUAL || op == S_IGUAL || op ==S_HASHTAG) return new_expression(BOOL, NULL);
    }
    else if(e.type == INT && e2.type == INT){
        if(op == S_MAIS || op == S_MENOS || op == S_VEZES || op == S_DIVISAO) return new_expression(INT, NULL);
        else if(op == S_OR || op == S_AND || op == S_MAIOR || op == S_MENOR ||
            op == S_MAIORIGUAL || op == S_MENORIGUAL || op == S_IGUAL || op ==S_HASHTAG) return new_expression(BOOL, NULL);
    }
    else if(e.type == BOOL && e2.type == BOOL){
        if(op == S_MAIS || op == S_MENOS || op == S_VEZES || op == S_DIVISAO) return new_expression(BOOL, NULL);
        else if(op == S_OR || op == S_AND || op == S_MAIOR || op == S_MENOR ||
            op == S_MAIORIGUAL || op == S_MENORIGUAL || op == S_IGUAL || op ==S_HASHTAG) return new_expression(BOOL, NULL);
    }
    else if(e.type == CHAR && e2.type == CHAR){
        if(op == S_MAIS || op == S_MENOS || op == S_VEZES || op == S_DIVISAO) return new_expression(CHAR, NULL);
        else if(op == S_OR || op == S_AND || op == S_MAIOR || op == S_MENOR ||
            op == S_MAIORIGUAL || op == S_MENORIGUAL || op == S_IGUAL || op ==S_HASHTAG) return new_expression(BOOL, NULL);
    }
    error2(7);
}

void comp_atrib(char tipo, char tipo2){
    /* int é compatível com int, e char é compatível com char;
    int é compatível com char, e vice-versa;
    O tipo implícito de uma expressão relacional (ex.: a>=b) é bool, que é
    compatível com o tipo int; em uma variável do tipo bool, um valor igual a 0 (zero) 
    indica falso lógico e um valor inteiro diferente de zero indica verdadeiro lógico;*/
    if(tipo == REAL && tipo2 != REAL || tipo != REAL && tipo2 == REAL || tipo == BOOL && tipo2 == INT ) error2(9);
}

void validar_funcao(int position){
    int i = -1;
    if(!identifier()) error(24);
    i = find_value_by_name(t_string());
    if(i == -1) error2(4);
    if(value_at(++position).valor->categoria != PARAM) error2(6);
    if(value_at(position).valor->tipo != value_at(i).valor->tipo) error2(5);
    while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
        get_next();
        get_next();
        if(!identifier()) error(25);
        i = find_value_by_name(t_string());
        if(i == -1) error2(4);
        if(value_at(++position).valor->categoria != PARAM) error2(6);
        if(value_at(position).valor->tipo != value_at(i).valor->tipo) error2(5);
    }
}

// FUNCOES SINTATICAS

void var(){
    item i;
    while(type()){
        int tipo = t_char();
        get_next();
        if(!identifier()) error(5);
        i = new_item(t_string(), VARIAVEL, tipo, GLOBAL);
        check_identifier(i);
        push_back(i);
        while(get_next() && operator() == VIRGULA){
            get_next();
            if(!identifier()) error(3);
            i = new_item(t_string(), VARIAVEL, tipo, GLOBAL);
            check_identifier(i);
            push_back(i);
        }
    }
    if(!(reserved() == ENDVAR)) error(4);
}

bool declaration(){
    item i;
    if(!type_ahead()) return false;
    get_next();
    int tipo = t_char();
    get_next();
    if(!identifier()) error(5);
    i = new_item(t_string(), VARIAVEL, tipo, LOCAL);
    check_identifier(i);//olha na tabela de simbolos
    push_back(i);
    while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
        get_next();
        get_next();
        if(!identifier()) error(5);
        i = new_item(t_string(), VARIAVEL, tipo, LOCAL);
        check_identifier(i);
        push_back(i);
    }
    return true;
}

bool param(int position){
    get_next();
    if(!(operator() == ABRE_PARENTESE)) error(7);
    if(position == -1){
        if(type_ahead()){
            get_next();
            int tipo = t_char();
            item i;
            get_next();
            if(!identifier()) error(5);
            i = new_item(t_string(), PARAM, tipo, LOCAL);
            check_identifier(i);
            push_back(i);
            while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
                get_next();
                get_next();
                if(!type()) error(14);
                tipo = t_char();
                get_next();
                if(!identifier()) error(5);
                i = new_item(t_string(), PARAM, tipo, LOCAL);
                check_identifier(i);
                push_back(i);
            }
        }
    }
    else{
        if(type_ahead()){
            get_next();
            int tipo = t_char();
            item i;
            get_next();
            if(!identifier()) error(5);
            i = new_item(t_string(), PARAM, tipo, LOCAL);
            check_identifier(i);
            if(value_at(++position).valor->categoria != PARAM) error2(2);
            if(value_at(position).valor->tipo != i.valor->tipo) error2(3);
            alter_at(position, i);
            while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
                get_next();
                get_next();
                if(!type()) error(18);
                tipo = t_char();
                get_next();
                if(!identifier()) error(5);
                i = new_item(t_string(), PARAM, tipo, LOCAL);
                check_identifier(i);
                if(value_at(++position).valor->categoria != PARAM) error2(2);
                if(value_at(position).valor->tipo != i.valor->tipo) error2(3);
                alter_at(position, i);
            }
        }
        else if(value_at(++position).valor->categoria == PARAM) error2(2);
    }
    get_next();
    if(!(operator() == FECHA_PARENTESE)) error(8);
    return true;
}

bool function(){
    if(type()){
        item ident;
        int tipo = t_char();
        get_next();
        if(!identifier()) error(6);
        ident = new_item(t_string(), FUNCTION, tipo, GLOBAL);
        check_identifier(ident);
        while(declaration());
        get_next();
        while(cmd() && get_next());
        pop_until_param();
        if(!(reserved() == ENDFUNC)) error(9);
        return true;
    }
    else if(reserved() == FWD){
        if(t_ahead.cat == RESERVED && t2_char() == PROC) return procedure();
        get_next();
        if(!type()) error(30);
        int tipo = t_char();
        item i;
        get_next();
        if(!identifier()) error(6);
        i = new_item(t_string(), FUNCTION_SIGNATURE, tipo, GLOBAL);
        check_identifier(i);
        push_back(i);
        get_next();
        if(!(operator() == ABRE_PARENTESE)) error(7);
        get_next();
        if(type()){
            int tipo = t_char();
            item i = new_item(NULL, PARAM, tipo, LOCAL);
            push_back(i);
            while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
                get_next();
                get_next();
                if(!type()) error(13);
                i = new_item(NULL, PARAM, tipo, LOCAL);
                push_back(i);
            }
        }
        else if(reserved() != NOPARAM) error(13);
        get_next();
        if(!(operator() == FECHA_PARENTESE)) error(8);
        return true;
    }
    return false;
}

bool procedure(){
    item ident;
    if(reserved() == PROC){
        get_next();
        if(!identifier()) error(10);
        ident = new_item(t_string(), PROCEDURE, -1, GLOBAL);
        check_identifier(ident);
        while(declaration());
        get_next();
        while(cmd() && get_next());
        pop_until_param();
        if(!(reserved() == ENDPROC)) error(11);
        return true;
    }
    else if(reserved() == FWD){
        get_next();
        if(reserved() != PROC) error(12);
        get_next();
        if(!identifier()) error(10);
        ident = new_item(t_string(), PROCEDURE_SIGNATURE, -1, GLOBAL);
        check_identifier(ident);
        push_back(ident);
        get_next();
        if(operator() != ABRE_PARENTESE) error(7);
        get_next();
        if(type()){
            ident = new_item(NULL, PARAM, t_char(), LOCAL);
            push_back(ident);
            while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
                get_next();
                get_next();
                if(!type) error(14);
                ident = new_item(NULL, PARAM, t_char(), LOCAL);
                push_back(ident);
            }
        }
        else if(reserved() != NOPARAM) error(13);
        get_next();
        if(operator() != FECHA_PARENTESE) error(8);
        return true;
    }
    return false;
}

Expression factor(){
    //printf("fator\n");
    Expression e;
    if(ct_car() || ct_int() || ct_real()){
        e = new_expression(const_type(t.cat), NULL);
        return e;
    }
    else if(identifier()){
        int position = find_value_by_name(t_string());
        if(position == -1) error2(4);
        e = new_expression(value_at(position).valor->tipo, NULL);
        if(!(t_ahead.cat == OPERADOR && t2_char() == ABRE_PARENTESE)) return e;
        get_next();
        get_next();
        /* PL não aceita a passagem de valores escalares (constantes)
        if(!expr()) error(24);
        while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
            get_next();
            get_next();
            if(!expr()) error(25);
        }
        */
        validar_funcao(position);
        get_next();
        if(!(t.cat == OPERADOR && t_char() == FECHA_PARENTESE)) error(8);
        return e;
    }
    else if(t.cat == OPERADOR && t_char() == ABRE_PARENTESE){
        get_next();
        e = expr();
        if(e.type == -1) error2(22);
        get_next();
        if(!(t.cat == OPERADOR && t_char() == FECHA_PARENTESE)) error(8);
        return e;
    }
    else if(t.cat == LOGICO && t_char() == NOT){
        get_next();
        e = factor();
        if(e.type == -1) error2(44);
        e.type = BOOL;
        return e;
    }
    return new_expression(-1, NULL);
}

Expression term(){
    Expression e, e2;
    //printf("termo\n");
    //if(e = factor() && e.type == -1) error(43);
    //printf("retorna do fator\n");
    e = factor();
    if((t_ahead.cat == OPERADOR && (t2_char() == VEZES || t2_char() == DIVISAO)) || 
        (t_ahead.cat == LOGICO && t2_char() == AND)){
        char op = expression_op();
        if(op == -1) error2(44);
        get_next();
        get_next();
        e2 = factor();
        if(e2.type == -1) error(25);
        return comp_expression(e, op, e2);
        //printf("retorna do fator\n");
    }
    return e;
}

Expression simple_expr(){
    Expression e, e2;
    //printf("expressao simples\n");
    if(t.cat == OPERADOR && (t_char() == MAIS || t_char() == MENOS)){
        get_next();
        e = term();
        if(e.type == -1) error(26);
        return e;
    }
    e = term();
    while((t_ahead.cat == OPERADOR && (t2_char() == MAIS || t2_char() == MENOS)) || (t_ahead.cat == LOGICO
        && t2_char() == OR)){
        char op = expression_op();
        get_next();
        get_next();
        e2 = term();
        if(e2.type == -1) error(27);
        e = comp_expression(e, op, e2);
    }
    return e;
}

bool op_rel(){
    return t_ahead.cat == LOGICO && t2_char() < 6;
}

Expression expr(){
    //printf("expressao\n");
    Expression e, e2;
    e = simple_expr();
    if(e.type == -1) return e;
    if(op_rel()){
        char op = expression_op();
        get_next();
        get_next();
        e2 = simple_expr();
        if(e2.type == -1) error(28);
        e = comp_expression(e, op, e2);
    }
    return e;
}

bool attribution(){
    int position = -1;
    Expression e;
    if(!identifier()) return false;
    position = find_value_by_name(t_string());
    get_next();
    if(!(operator() == ATRIBUICAO)) error(23);
    get_next();
    e = expr();
    if(e.type == -1) error(24);
    comp_atrib(value_at(position).valor->tipo, e.type);
    return true;
}

bool cmd(){
    int position = -1;
    switch(t.cat){
        case RESERVED:
            switch(t_char()){
                Expression e;
                case IF:
                    get_next();
                    if(!(operator() == ABRE_PARENTESE)) error(7);
                    get_next();
                    e = expr();
                    if(e.type == -1) error(41);
                    if(e.type != BOOL) error2(8);
                    get_next();
                    if(!(operator() == FECHA_PARENTESE)) error(8);
                    get_next();
                    if(!cmd()) error(35);
                    get_next();
                    while(cmd()) get_next();
                    if(reserved() == ELSE){
                        get_next();
                        if(!cmd()) error(36);
                        get_next();
                        while(cmd()) get_next();
                    }
                    if(!(reserved() == ENDIF)) error(16);
                    return true;
                case WHILE:
                    get_next();
                    if(!(operator() == ABRE_PARENTESE)) error(7);
                    get_next();
                    e = expr();
                    if(e.type == -1) error(81);
                    if(e.type != BOOL) error2(8);
                    get_next();
                    if(!(operator() == FECHA_PARENTESE)) error(8);
                    get_next();
                    if(!cmd()) error(37);
                    get_next();
                    while(cmd()) get_next();
                    if(!(reserved() == ENDWHILE)) error(17);
                    return true;
                case FOR:
                    get_next();
                    if(!(operator() == ABRE_PARENTESE)) error(7);
                    if(t_ahead.cat == IDENTIFIER){
                        get_next();
                        if(!attribution()) error(32);
                    }
                    get_next();
                    if(!(operator() == VIRGULA)) error(33);
                    get_next();
                    e = expr();
                    if(e.type == -1) error(42);
                    if(e.type != BOOL) error2(8);
                    get_next();
                    if(!(operator() == VIRGULA)) error(33);
                    if(t_ahead.cat == IDENTIFIER){
                        get_next();
                        if(!attribution()) error(33);
                    }
                    get_next();
                    if(!(operator() == FECHA_PARENTESE)) error(8);
                    get_next();
                    if(!cmd()) error(38);
                    get_next();
                    while(cmd()) get_next();
                    if(!(reserved() == ENDFOR)) error(34);
                    return true;
                case RETURN:
                    if(inside_function){
                        //position = find_function_procudure();
                        //if(position == -1) execution_erro(0);
                        if(value_at(inside_function).valor->categoria == PROCEDURE) return true;
                    }
                    get_next();
                    if(!(operator() == ABRE_PARENTESE)) error(7);
                    get_next();
                    e = expr();
                    if(e.type == -1) error(23);
                    get_next();
                    if(!(operator() == FECHA_PARENTESE)) error(8);
                    if(position != -1) 
                        if(value_at(inside_function).valor->tipo != e.type) error2(15);
                    return true;
                case CALL:
                    position = -1;
                    get_next();
                    if(!identifier()) error(18);
                    position = find_value_by_name(t_string());
                    if(position == -1) error2(4);
                    if(value_at(position).valor->categoria != PROCEDURE &&
                        value_at(position).valor->categoria != PROCEDURE_SIGNATURE) error2(10);
                    get_next();
                    if(!(operator() == ABRE_PARENTESE)) error(7);
                    get_next();
                    validar_funcao(position);
                    get_next();
                    if(!(operator() == FECHA_PARENTESE)) error(8);
                    return true;
                case KEYBOARD:
                    position = -1;
                    get_next();
                    if(!identifier()) error(21);
                    position = find_value_by_name(t_string());
                    if(position == -1) error2(11);
                    if(value_at(position).valor->categoria != VARIAVEL) error2(12);
                    while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
                        get_next();
                        get_next();
                        if(!identifier()) error(21);
                        position = find_value_by_name(t_string());
                        if(position == -1) error2(11);
                        if(value_at(position).valor->categoria != VARIAVEL) error2(12);
                    }
                    return true;
                case DISPLAY:
                    position = -1;
                    get_next();
                    if(!(identifier() || ct_int() || ct_car() || ct_real() || ct_string())) error(20);
                    if(identifier()){
                        position = find_value_by_name(t_string());
                        if(position == -1) error2(11);
                        if(value_at(position).valor->categoria != VARIAVEL 
                            && value_at(position).valor->categoria != PARAM) error2(13);
                    }
                    //get_next();
                    if(t_ahead.cat == RESERVED && t2_char() == DUP){
                        get_next();
                        get_next();
                        if(!(identifier() || ct_int())) error(22);
                        if(identifier()){
                            position = find_value_by_name(t_string());
                            if(position == -1) error2(11);
                            if(value_at(position).valor->categoria != VARIAVEL 
                                && value_at(position).valor->categoria != PARAM) error2(13);
                        }
                    }
                    while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
                        get_next();
                        get_next();
                        if(!(identifier() || ct_int() || ct_car() || ct_real() || ct_string())) error(20);
                        if(identifier()){
                            position = find_value_by_name(t_string());
                            if(position == -1) error2(11);
                            if(value_at(position).valor->categoria != VARIAVEL 
                                && value_at(position).valor->categoria != PARAM) error2(13);
                        }
                        if(t_ahead.cat == RESERVED && t2_char() == DUP){
                            get_next();
                            get_next();
                            if(!(identifier() || ct_int())) error(22);
                            if(identifier()){
                                position = find_value_by_name(t_string());
                                if(position == -1) error2(11);
                                if(value_at(position).valor->categoria != VARIAVEL 
                                    && value_at(position).valor->categoria != PARAM) error2(13);
                            }
                        }
                    }
                    return true;
                default:
                    return false;
            }
        case IDENTIFIER:
            if(t_ahead.cat == OPERADOR && t2_char() == ATRIBUICAO){
                return attribution();
            }
            return false;
        case OPERADOR:
            return t_char() == PONTO_VIRGULA;
        default:
            return false;
    }
}

void prog(){
    if(reserved() == PROG){
        get_next();
        if(!cmd()) error(39);
        get_next();
        while(cmd()) get_next();
        if(!(reserved() == ENDPROG)) error(2);
    } 
    else error(31);
}

void start(){
    table = new_vector(NULL, 10);
    item i = new_item(NULL, -1, -1, -1);
    push_back(i);
    get_first_token();
    if(reserved() == PL){
        get_next();
        if(!identifier()) error(1);
        get_next();
        if(reserved() == VAR){
            get_next();
            var();
            get_next();
        }
        inside_function = -1;
        while(function() || procedure()) get_next();
        inside_function = 0;
        prog();
    }
    else error(0);
}

void analise(){
    start();
    printf("Programa valido\n");
    //free_vector(table);
}