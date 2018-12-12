#include "sintatico.h"

token t, t_ahead;
myvector table;
char expression_type;
bool inside_function = 0;

char ErrosSintaticos[][70] = {"Palavra reservada de inicializacao faltando", "Nome do programa faltando", "endprog faltando",
                "declaracao de variavel incorreta", "endvar faltando", "identificador depois do tipo faltando",
                "identificador depois do tipo da funcao faltando", "Faltando abre parenteses", "Faltando fecha parenteses",
                "Faltando endfunc", "identificador do procedimento faltando", "Faltando endproc", "Faltando proc depois de fwd",
                "Faltando tipo depois do abre parenteses do procedimento", "Faltando tipo depois da virgula", "endif faltando",
                "endwhile faltando", "Faltando identificador depois de instrucao call",
                "depois da virgula tem que vir outra expressao na instrucao call", "constante ou identificador faltando depois da instrucao display",
                "identificador faltando depois da instrucao keyboard", "identificador ou constante inteira faltando depois da instrucao dup",
                "operador igual depois do identificador faltando", "expressao depois do identificador igual faltando ou invalida",
                "expressao faltando depois de abre parentesis", "expressao faltando depois da virgula", 
                "fator depois da virgula faltando", "termo faltando depois de operador", "faltando termo depois do operador",
                "faltando expressao simples depois do operador relacional", "Faltando o tipo depois do fwd", "prog faltando",
                "Faltou atribuicao", "Faltou virgula", "Faltou endfor", "Deve haver um comando depois da instrucao  if", "Deve haver um comando depois da instrucao  else", 
                "Deve haver um comando depois da instrucao  while", "Deve haver um comando depois da instrucao for",
                "Deve haver um comando depois da instrucao prog", "Faltando expressao depois do abre parentesis",
                "Faltando expressao depois da virgula", "faltando expressao depois do igual"
};

char ErrosSemanticos[][70] = {"Esse Nome de identificador ja esta em uso", "O tipo da funcao declarada nao e igual ao da assinatura",
                    "A quantidade de parametro(s) e diferente da assinatura", "Tipo do parametro diferente da assinatura",
                    "Variavel, funcao ou procedimento nao foi declarado", "Argumento nao compativel com a funcao", 
                    "Quantidade invalida de argumentos", "tipos da expressao incompativeis", "A expressao nao e um bool",
                    "atribuicao invalida", "A instrucao call so pode chamar um procedimento", "Variavel nao foi declarada",
                    "a instrucao keyborad espera uma variavel", "o(s) identificador(es) da instrucao display deve(m) ser uma variavel",
                    "identificador invalido"
};

// FUNCOES DE ERRO

void error(int e){
    printf("Erro Sintatico %i na linha %i e na coluna %i: %s", e, get_linha(), get_coluna(), ErrosSintaticos[e]);
    exit(1);
}

void error2(int e){
    printf("Erro Semantico %i na linha %i e na coluna %i: %s", e, get_linha(), get_coluna(), ErrosSemanticos[e]);
    exit(1);
}


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
    i.valor->nome = NULL;
    if(nome){
        i.valor->nome = (char*) malloc(strlen(nome));
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
    v.tam = tam + 10;
    v.current = -1;
    for(int i=0; i<tam && value != NULL; i++) v.value[i].valor = value;
    return v;
}

void free_vector(){
    for(int i=0; i<table.current; i++) free_item(table.value[i]);
    free(table.value);
}

void push_back(item value){
    if(table.current == table.tam){
        print_table();
        table.tam += 10;
        item *values, *valor;
        values = valor = (item *) malloc(sizeof(item) * table.tam);
        memcpy(values, table.value, sizeof(item) * table.tam);
        
        print_table();
        free(table.value);
        table.value = values;
    }
    table.value[++table.current] = value;
}

void pop_back(){
    if(table.current >= 0) free_item(table.value[table.current--]);
}

void pop_until_param(){
    for(int i = table.current; i>=0 && table.value[i].valor->categoria == VARIAVEL &&
        table.value[i].valor->scopo == LOCAL; i--) pop_back();
}

item value_at(int p){
    if(p <= table.current && p>=0) return(table.value[p]);
    printf("out of range: %i", p);
    item i;
    i.valor->nome = NULL;
    i.valor->tipo = i.valor->categoria = i.valor->scopo = -1;
    return i;
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
        int position = find_until_function_by_name(s);
        if(position != -1) return position;
    }
    return find_no_param_by_name(s);
}

int find_no_param_by_name(char* s){
    //print_table();
    printf("Gloria\n");
    for(int i=table.current; i>=0; i--){ if(table.value[i].valor->nome && strcmp(table.value[i].valor->nome, s) == 0 && 
        table.value[i].valor->categoria != PARAM) return i;
    //printf("%s\n", table.value[i].valor->nome?table.value[i].valor->nome:"");
    }
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
    for(int i=table.current; i>=0 && table.value[i].valor->scopo == LOCAL; i--)
        if(table.value[i].valor->nome && strcmp(table.value[i].valor->nome, s) == 0) return i;
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

void print_table(){
    for(int i=0; i<=table.current; i++) 
        printf("| Nome : %s | Tipo: %i | Categoria: %i | Scopo: %i|\n", 
            table.value[i].valor->nome ? table.value[i].valor->nome : "",
            table.value[i].valor->tipo, table.value[i].valor->categoria,
            table.value[i].valor->scopo);
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
            if(position != -1){
                if(!(value_at(position).valor->categoria == FUNCTION_SIGNATURE)) error2(0);
                if(!(value_at(position).valor->tipo == ident.valor->tipo)) error2(1);
                param(position);
                alter_at(position, ident);
            }
            else{
                push_back(ident);
                param(-1);
            }
            break;
        case PROCEDURE:
            position = find_value_by_name(ident.valor->nome);
            if(position != -1){
                if(!(value_at(position).valor->categoria == PROCEDURE_SIGNATURE)) error2(0);
                param(position);
                alter_at(position, ident);
            }
            else{
                push_back(ident);
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

void comp_expression(char tipo){
    /*Se e é uma expressão na forma e1 + e2, e1 - e2, e1 * e2, e1 / e2, ou -e1, então o
    tipo de e é compatível com os tipos dos elementos da expressão, retritos a int,
    char e real, ou seja, em “e1 + e2” se e1 for char e e2 for int, a operação é
    possível porque estes tipos possuem compatibilidade entre si e o tipo da
    expressão fica sendo int; por outro lado, se e1 for um int e e2 for um real um
    conflito de tipos é estabelecido e uma mensagem de erro deve ser emitida;
    Se e é uma expressão na forma e1 >= e2, e1 <= e2, e1 > e2, e1 < e2, e1 == e2, ou
    e1 # e2 então o tipo de e é bool.*/
    if(expression_type == CHAR && tipo == INT) expression_type = INT; 
    else if(expression_type == BOOL && tipo == INT || expression_type == INT && tipo == BOOL) expression_type = INT;
    else if(expression_type == REAL && tipo != REAL || expression_type != REAL && tipo == REAL) error2(7);
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
    if(value_at(++position).valor->categoria != PARAM) error2(6);
    if(value_at(position).valor->tipo != value_at(i).valor->tipo) error2(5);
    while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
        get_next();
        get_next();
        if(!identifier()) error(25);
        i = find_value_by_name(t_string());
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
    get_next();
    if(position == -1){
        if(type()){
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
        if(type()){
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
                //printf()
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
        else if(reserved() != NOPARAM) error(43);
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
        ident = new_item(t_string(), PROCEDURE, BOOL, GLOBAL);
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
        get_next();
        if(operator() != ABRE_PARENTESE) error(7);
        get_next();
        if(!type) error(13);
        ident = new_item(NULL, PARAM, t_char(), LOCAL);
        push_back(ident);
        while(get_next() && operator() == VIRGULA){
            get_next();
            if(!type) error(14);
            ident = new_item(NULL, PARAM, t_char(), LOCAL);
            push_back(ident);
        }
        if(operator() != FECHA_PARENTESE) error(8);
        return true;
    }
    return false;
}

bool factor(){
    //printf("fator\n");
    if(ct_car() || ct_int() || ct_real()){
        if(expression_type = -1) expression_type = t.cat;
        else comp_expression(t.cat);
        return true;
    }
    else if(identifier()){
        int position = find_value_by_name(t_string());
        if(position == -1) error2(4);
        if(expression_type == -1) expression_type = value_at(position).valor->tipo;
        else comp_expression(value_at(position).valor->tipo);
        if(!(t_ahead.cat == OPERADOR && t2_char() == ABRE_PARENTESE)) return true;
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
        return true;
    }
    else if(t.cat == OPERADOR && t_char() == ABRE_PARENTESE){
        print_token(t);
        get_next();
        if(!expr()) error(24);
        get_next();
        if(!(t.cat == OPERADOR && t_char() == FECHA_PARENTESE)) error(8);
        return true;
    }
    else{
        if(!(t.cat == LOGICO && t_char() == NOT)) return false;
        get_next();
        expression_type = BOOL;
        return factor();
    }
}

bool term(){
    //printf("termo\n");
    if(!factor()) return false;
    //printf("retorna do fator\n");
    if((t_ahead.cat == OPERADOR && (t2_char() == VEZES || t2_char() == DIVISAO)) || 
        (t_ahead.cat == LOGICO && t2_char() == NOT)){
        get_next();
        get_next();
        if(!factor()) error(27);
        //printf("retorna do fator\n");
    }
    return true;
}

bool simple_expr(){
    //printf("expressao simples\n");
    if(t.cat == OPERADOR && (t_char() == MAIS || t_char() == MENOS)){
        get_next();
        if(!term()) error(28);
    }
    else if(!term()){
        return false;
    }
    while((t_ahead.cat == OPERADOR && (t2_char() == MAIS || t2_char() == MENOS)) || (t_ahead.cat == LOGICO
        && t2_char() == OR)){
        get_next();
        get_next();
        if(!term()) error(29);
    }
    return true;
}

bool op_rel(){
    return t_ahead.cat == LOGICO && t2_char() < 6;
}

bool expr(){
    //printf("expressao\n");
    if(!simple_expr()) return false;
    if(op_rel()){
        get_next();
        get_next();
        if(!simple_expr()) error(30);
        expression_type = BOOL;
    }
    return true;
}

bool attribution(){
    int position = -1;
    if(!identifier()) return false;
    position = find_value_by_name(t_string());
    get_next();
    if(!(operator() == ATRIBUICAO)) error(23);
    get_next();
    expression_type = -1;
    if(!expr()) error(24);
    comp_atrib(value_at(position).valor->tipo, expression_type);
    return true;
}

bool cmd(){
    int position = -1;
    switch(t.cat){
        case RESERVED:
            switch(t_char()){
                case IF:
                    get_next();
                    if(!(operator() == ABRE_PARENTESE)) error(7);
                    get_next();
                    if(!expr()) error(41);
                    expression_type = -1;
                    if(expression_type != BOOL) error2(8);
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
                    expression_type = -1;
                    if(!expr()) error(81);
                    if(expression_type != BOOL) error2(8);
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
                    expression_type = -1;
                    if(!expr()) error(42);
                    if(expression_type != BOOL) error2(8);
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
                    get_next();
                    if(!(operator() == ABRE_PARENTESE)) error(7);
                    get_next();
                    expression_type = -1;
                    if(!expr()) error(81);
                    get_next();
                    if(!(operator() == FECHA_PARENTESE)) error(8);
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
                    if(t_ahead.cat == DUP){
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
                        if(t_ahead.cat == DUP){
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
    printf("TABELA DE DEUX\n\n");
    print_table();
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
        inside_function = true;
        while(function() || procedure()) get_next();
        inside_function = false;
        prog();
    }
    else error(0);
}

void analise(){
    start();
    free_vector(table);
    printf("Programa valido\n");
}