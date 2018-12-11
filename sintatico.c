#include "sintatico.h"

token t, t_ahead;
myvector table;
char expression_type;

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
                "Faltando expressao depois da virgula"
};

char ErrosSemanticos[][70] = {"Esse Nome de identificador ja esta em uso", "O tipo da funcao declarada nao e igual ao da assinatura",
                    "A quantidade de parametro(s) e diferente da assinatura", "Tipo do parametro diferente da assinatura",
                    "Variavel, funcao ou procedimento nao foi declarado", "Argumento nao compativel com a funcao", 
                    "Quantidade invalida de argumentos", "tipos da expressao incompativeis", "A expressao nao e um bool",
                    "atribuicao invalida", "A instrucao call nao pode chamar uma funcao"
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
    free(i.valor);
}

item new_item(char* nome, int categoria, int tipo, int scopo){
    item i;
    i.valor = (simbolo *) malloc(sizeof(simbolo));
    i.valor->nome = nome;
    i.valor->categoria = categoria;
    i.valor->tipo = tipo;
    i.valor->scopo = scopo;
    return i;
}

myvector new_vector(simbolo* value, int tam){
    myvector v;
    v.value = (item *) malloc(sizeof(item) * tam + 10);
    v.tam = tam + 10;
    v.current = 0;
    for(int i=0; i<tam && value != NULL; i++) v.value[i].valor = value;
    return v;
}

void free_vector(){
    for(int i=0; i<table.current; i++) free_item(table.value[i]);
    free(table.value);
}

void push_back(item value){
    if(table.current <= table.tam){
        table.tam += 10;
        item *values, *valor;
        values = valor = (item *) malloc(sizeof(item) * table.tam);
        memcpy(values, table.value, sizeof(item) * table.tam);
        free(table.value);
        table.value = values;
    }
    table.value[table.current++] = value;
}

void pop_back(){
    if(table.current >= 0) free_item(table.value[table.current--]);
}

void pop_until_param(){
    while(table.value[table.current].valor->categoria == VARIAVEL && 
        table.value[table.current].valor->scopo == LOCAL) pop_back();
}

item value_at(int p){
    if(p < table.current) return(table.value[p]);
    else printf("out of range!");
}

item *find_value(item value){
    for(int i=0; i<table.current; i++) if(cmp_item(table.value[i], value)) return table.value + i;
    return NULL;
}

item *find_value_by_name(char* s){
    for(int i=0; i<table.current; i++) if(strcmp(table.value[i].valor->nome, s) == 0) return table.value + i;
    return NULL;
}

item *find_value_by_name_local(char* s){
    for(int i=0; i<table.current && table.value[i].valor->scopo == LOCAL; i++) if(strcmp(table.value[i].valor->nome, s) == 0) return table.value + i;
    return NULL;
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
    free_token(t);
    t = t_ahead;
    t_ahead = get_token();
    print_token(t);
    return true;
}

// VERIFICACAO DO TIPO DO TOKEN

bool type(){
    return (t.cat == RESERVED && (t.cat == CHAR || t.cat == INT || t.cat == REAL
            || t.cat == BOOL));
}

bool type_ahead(){
    return (t_ahead.cat == RESERVED && (t_ahead.cat == CHAR || t_ahead.cat == INT || t_ahead.cat == REAL
            || t_ahead.cat == BOOL));
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
    item* ident2 = find_value_by_name(ident.valor->nome);
    switch(ident.valor->categoria){
        case VARIAVEL:
            item* ident2 = NULL;
            if(ident.valor->tipo == GLOBAL) ident2 = find_value_by_name(ident.valor->nome);
            else ident2 = find_value_by_name_local(ident.valor->nome);
            if(ident2 != NULL) error2(0);
            break;
        case FUNCTION_SIGNATURE:
            item* ident2 = find_value_by_name(ident.valor->nome);
            if(ident2 != NULL) error2(0);
            break;
        case PROCEDURE_SIGNATURE:
            item* ident2 = find_value_by_name(ident.valor->nome);
            if(ident2 != NULL) error2(0);
            break;
        case FUNCTION:
            item* ident2 = find_value_by_name(ident.valor->nome);
            if(ident2 != NULL){
                if(!(ident2->valor->categoria == FUNCTION_SIGNATURE)) error2(0);
                if(!(ident2->valor->tipo == ident.valor->tipo)) error2(1);
                param(ident2);
            }
            else{
                push_back(ident);
                param(NULL);
            }
            break;
        case PROCEDURE:
            item* ident2 = find_value_by_name(ident.valor->nome);
            if(ident2 != NULL){
                if(!(ident2->valor->categoria == PROCEDURE_SIGNATURE)) error2(0);
                param(ident2);
            }
            else{
                push_back(ident);
                param(NULL);
            }
            break;
        }
        case PARAM:
            item* ident2 = find_value_by_name_local(ident.valor->categoria);
            if(ident2 != NULL) error2(0);
            break;
        default:
            printf("Categoria invalida");
            break;
    }
}

void comp_expression(char tipo){
    if(expression_type == CHAR && tipo == INT) expression_type = INT; 
    else if(expression_type == REAL && tipo != REAL || expression_type != REAL && tipo == REAL) error2(7);
    else if(expression_type == BOOL && tipo == INT || expression_type == INT && tipo == BOOL) expression_type = INT;
}

void comp_atrib(char tipo, char tipo2){
    if(tipo == REAL && tipo2 != REAL || tipo != REAL && tipo2 == REAL || tipo == BOOL && tipo2 == INT ) error2(9);
}

void validar_funcao(item* i){
    item *i2;
    if(!identifier()) error(24);
    i2 = find_value_by_name(t_string());
    i++;
    if(i->valor->categoria != PARAM) error2(6);
    if(i->valor->tipo != i2->valor->tipo) error2(5);
    while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
        get_next();
        get_next();
        if(!identifier()) error(25);
        i2 = find_value_by_name(t_string());
        i++;
        if(i->valor->categoria != PARAM) error2(6);
        if(i->valor->tipo != i2->valor->tipo) error2(5);
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
            if(find_value(i) != NULL) error2(0);
            i = new_item(t_string(), VARIAVEL, tipo, GLOBAL);
            check_identifier(i);
            push_back(i);
        }
    }
    if(!(t.cat == RESERVED && t_char() == ENDVAR)) error(4);
}

bool declaration(){
    item i;
    if(!type_ahead()) return false;
    get_next();
    int tipo = t_char();
    get_next();
    if(!identifier()) error(5);
    i = new_item(t_string(), VARIAVEL, tipo, LOCAL);
    check_identifier(i);
    push_back(i);
    while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
        get_next();
        get_next();
        if(!identifier()) error(5);
        i = new_item(t_string(), VARIAVEL, tipo, LOCAL);
        if(find_value(i) != NULL) error2(0);
        check_identifier(i);
        push_back(i);
    }
    return true;
}

bool param(item *ident){
    get_next();
    if(!(operator() == ABRE_PARENTESE)) error(7);
    get_next();
    if(ident == NULL){
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
            ident++;
            if(ident->valor->categoria != PARAM) error2(2);
            if(ident->valor->tipo != i.valor->tipo) error2(3);
            free_item(*ident);
            *ident = i;
            ident++;
            while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
                get_next();
                get_next();
                if(!identifier()) error(5);
                i = new_item(t_string(), PARAM, tipo, LOCAL);
                check_identifier(i);
                if(ident->valor->categoria != PARAM) error2(2);
                if(ident->valor->tipo != i.valor->tipo) error2(3);
                free_item(*ident);
                *ident = i;
                ident++;
            }
        }
        else if((ident+1)->valor->categoria == PARAM) error2(2);
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
        while(declaration() && get_next());
        get_next();
        while(cmd() && get_next());
        pop_until_param();
        if(!(reserved() == ENDFUNC)) error(9);
        return true;
    }
    else if(t.cat == FWD){
        if(t_ahead.cat == RESERVED && t2_char() == PROC) return procedure();
        get_next();
        if(!type()) error(71);
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
        else if(reserved() != NOPARAM) error(83);
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
        while(declaration() && get_next());
        get_next();
        while(cmd() && get_next());
        pop_until_param();
        if(!(reserved() == ENDPROC)) error(11);
        return true;
    }
    else if(reserved() == FWD){
        get_next();
        if(reserved() == PROC) error(12);
        get_next();
        if(!identifier()) error(10);
        get_next();
        if(!(operator() == ABRE_PARENTESE)) error(7);
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
        if(!(operator() == FECHA_PARENTESE)) error(8);
        return true;
    }
    return false;
}

bool factor(){
    printf("fator\n");
    print_token(t);
    if(ct_car() || ct_int() || ct_real()){
        //print_token(t);
        if(expression_type = -1) expression_type = t.cat;
        else comp_expression(t.cat);
        return true;
    }
    else if(identifier()){
        item* ident = find_value_by_name(t_string());
        if(ident == NULL) error2(4);
        //print_token(t);
        if(expression_type = -1) expression_type = ident->valor->tipo;
        else expression_type = comp_expression(ident->valor->tipo);
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
        validar_funcao(ident);
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
    printf("termo\n");
    if(!factor()) return false;
    printf("retorna do fator\n");
    if((t_ahead.cat == OPERADOR && (t2_char() == VEZES || t2_char() == DIVISAO)) || 
        (t_ahead.cat == LOGICO && t2_char() == NOT)){
        get_next();
        get_next();
        if(!factor()) error(27);
        printf("retorna do fator\n");
    }
    return true;
}

bool simple_expr(){
    printf("expressao simples\n");
    if(t.cat == OPERADOR && (t_char() == MAIS || t_char() == MENOS)){
        get_next();
        if(!term()) error(28);
        printf("\ngloria a deux\n");
    }
    else if(!term()){
        printf("\ngloria\n");
        return false;
    }
    print_token(t);
    printf("\ngloria a deuxx\n");
    while((t_ahead.cat == OPERADOR && (t2_char() == MAIS || t2_char() == MENOS)) || (t_ahead.cat == LOGICO
        && t2_char() == OR)){
        printf("\ngloria irmaos\n");
        get_next();
        get_next();
        if(!term()) error(29);
        printf("\ngloria a deuxxx\n");
    }
    return true;
}

bool op_rel(){
    return t_ahead.cat == LOGICO && t2_char() < 6;
}

bool expr(){
    //expression_type = -1;
    printf("expressao\n");
    if(!simple_expr()) return tipo;
    printf("gloria\n");
    if(op_rel()){
        get_next();
        get_next();
        if(!simple_expr()) error(30);
        expression_type = BOOL;
    }
    return true;
}

bool attribution(){
    item *i = NULL;
    if(!identifier()) return false;
    i = find_value_by_name(t_string());
    //if(t_ahead.cat == OPERADOR && t2_char() == IGUAL){
    get_next();
    if(!(t.cat == OPERADOR && t_char() == IGUAL)) error(23);
    get_next();
    if(!expr()) error(24);
    comp_atrib(i->valor->tipo, expression_type);
    //}
    return true;
}

bool cmd(){
    item *i;
    switch(t.cat){
        case RESERVED:
            switch(t_char()){
                case IF:
                    get_next();
                    if(!(operator() == ABRE_PARENTESE)) error(7);
                    get_next();
                    if(!expr()) error(81);
                    if(expression_type != BOOL) error2(8);
                    get_next();
                    if(!(operator() == FECHA_PARENTESE)) error(8);
                    get_next();
                    if(!cmd()) error(76);
                    get_next();
                    while(cmd()) get_next();
                    if(reserved() == ELSE){
                        get_next();
                        if(!cmd()) error(77);
                        get_next();
                        while(cmd()) get_next();
                    }
                    if(!(reserved() == ENDIF)) error(16);
                    return true;
                case WHILE:
                    get_next();
                    if(!(operator() == ABRE_PARENTESE)) error(7);
                    get_next();
                    if(!expr()) error(81);
                    if(expression_type != BOOL) error2(8);
                    get_next();
                    if(!(operator() == FECHA_PARENTESE)) error(8);
                    get_next();
                    if(!cmd()) error(78);
                    get_next();
                    while(cmd()) get_next();
                    if(!(reserved() == ENDWHILE)) error(17);
                    return true;
                case FOR:
                    get_next();
                    if(!(operator() == ABRE_PARENTESE)) error(7);
                    if(t_ahead.cat == IDENTIFIER){
                        get_next();
                        if(!attribution()) error(73);
                    }
                    get_next();
                    if(!(operator() == VIRGULA)) error(74);
                    get_next();
                    if(!expr()) error(82);
                    if(expression_type != BOOL) error2(8);
                    get_next();
                    if(!(operator() == VIRGULA)) error(74);
                    if(t_ahead.cat == IDENTIFIER){
                        get_next();
                        if(!attribution()) error(73);
                    }
                    get_next();
                    if(!(operator() == FECHA_PARENTESE)) error(8);
                    get_next();
                    if(!cmd()) error(79);
                    get_next();
                    while(cmd()) get_next();
                    if(!(reserved() == ENDFOR)) error(75);
                    return true;
                case RETURN:
                    get_next();
                    if(!(operator() == ABRE_PARENTESE)) error(7);
                    get_next();
                    if(!expr()) error(81);
                    get_next();
                    if(!(operator() == FECHA_PARENTESE)) error(8);
                    return true;
                case CALL:
                    get_next();
                    if(!identifier()) error(18);
                    i = find_value_by_name(t_string());
                    if(i->valor->categoria == FUNCTION) error2(10);
                    if(i == NULL) error2(4);
                    if(!(operator() == ABRE_PARENTESE)) error(7);
                    get_next();
                    validar_funcao();
                    get_next();
                    if(!(operator() == FECHA_PARENTESE)) error(8);
                    return true;
                case KEYBOARD:
                    get_next();
                    if(!identifier()) error(21);
                    while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
                        get_next();
                        get_next();
                        if(!identifier()) error(21);
                    }
                    return true;
                case DISPLAY:
                    get_next();
                    if(!(identifier() || ct_int() || ct_car() || ct_real() || ct_string())) error(20);
                    get_next();
                    if(t.cat == DUP){
                        get_next();
                        if(!(identifier() || ct_int())) error(22);
                    }
                    while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
                        get_next();
                        get_next();
                        if(!(identifier() || ct_int() || ct_car() || ct_real() || ct_string())) error(20);
                        if(t_ahead.cat == DUP){
                            get_next();
                            get_next();
                            if(!(identifier() || ct_int())) error(22);
                        }
                    }
                    return true;
                default:
                    return false;
            }
        case IDENTIFIER:
            if(t_ahead.cat == OPERADOR && t2_char() == IGUAL){
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
        if(!cmd()) error(80);
        get_next();
        while(cmd()) get_next();
        if(!(reserved() == ENDPROG)) error(2);
    } 
    else error(72);
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
        while(function() || procedure()) get_next();
        prog();
    }
    else error(0);
}

void analise(){
    start();
    //if(expr()) printf("Gloria... Gloria Deux\nExpressao valida\n");

}