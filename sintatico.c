#include "sintatico.h"

token t;
char Erros[][70] = {"Palavra reservada de inicializacao faltando", "Nome do programa faltando", "endprog faltando",
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
                "faltando expressao simples depois do operador relacional"
};

bool cmp_item(item x, item y){
    return (strcmp(x.valor->nome, y.valor->nome) && x.valor->scopo == y.valor->scopo && 
        x.valor->categoria == y.valor->categoria && x.valor->tipo == y.valor->tipo);
}

void free_item(item i){
    free(i.valor);
}

myvector new_vector(simbolo* value, int tam){
    myvector v;
    v.value = (item *) malloc(sizeof(item) * tam + 10);
    v.tam = tam + 10;
    v.current = 0;
    for(int i=0; i<tam && value != NULL; i++) v.value[i].valor = value;
    return v;
}

void free_vector(myvector v){
    for(int i=0; i<v.current; i++) free_item(v.value[i]);
    free(v.value);
}

void push_back(myvector v, item value){
    if(v.current <= v.tam){
        v.tam += 10;
        item *values, *valor;
        values = valor = (item *) malloc(sizeof(item) * v.tam);
        memcpy(values, v.value, sizeof(item) * v.tam);
        free(v.value);
        v.value = values;
    }
    v.value[v.current++] = value;
}

void pop_back(myvector v){
    if(v.current >= 0) free_item(v.value[v.current--]);
}

item value_at(myvector v, int p){
    if(p < v.current) return(v.value[p]);
    else printf("out of range!");
}

item *find_value(myvector v, item value){
    for(int i=0; i<v.current; i++) if(cmp_item(v.value[i], value)) return v.value + i;
    return NULL;
}

char t_char(){
    return *cchar(t.value);
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

void error(int e){
    printf("Erro Sintatico na linha %i e na coluna %i: %s", Erros[e], get_linha(), get_coluna());
    exit(1);
}

bool get_next(){
    free_token(t);
    t = get_token();
    print_token(t);
    return true;
}

bool type(){
    return (t.cat == RESERVED && (t.cat == CHAR || t.cat == INT || t.cat == REAL
            || t.cat == BOOL));
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

void var(){
    while(type()){
        get_next();
        if(identifier()){
            //push var
            while(get_next() && t.cat == OPERADOR && t_char() == VIRGULA){
                get_next();
                if(!identifier()) error(3);
            }
            if(!(t.cat == RESERVED && t_char() == ENDVAR)) error(4);
        }
        else error(5);
    }
    get_next();
}

bool declaration(){
    if(!type()) return false;
    get_next();
    if(!identifier()) error(5);
    //push
    return true;
}

bool param(){
    if(type()){
        get_next();
        if(!identifier()) error(5);
        //push param
        while(get_next() && t.cat == OPERADOR && t_char() == VIRGULA){
            get_next();
            if(!identifier()) error(5);
        }
    }
    return true;
}

bool function(){
    if(type()){
        get_next();
        if(!identifier()) error(6);
        //push name func
        get_next();
        if(!(t.cat == OPERADOR && t_char() == ABRE_PARENTESE)) error(7);
        get_next();
        param();
        if(!(t.cat == OPERADOR && t_char() == FECHA_PARENTESE)) error(8);
        get_next();
        while(declaration()){
            //push
            while(get_next() && t.cat == OPERADOR && t_char() == VIRGULA){
                get_next();
                if(!identifier()) error(5);
                //push
            }
        }
        while(cmd());
        if(!(t.cat == RESERVED && t_char() == ENDFUNC)) error(9);
        get_next();
        return true;
    }
    else if(t.cat == FWD){
        get_next();
        if(!type()) return procedure();
        get_next();
        if(!identifier()) error(6);
        get_next();
        if(!(t.cat == OPERADOR && t_char() == ABRE_PARENTESE)) error(7);
        get_next();
        if(type()){
            while(get_next() && t.cat == OPERADOR && t_char() == VIRGULA) if(!type()) error(13);
        }
        if(!(t.cat == OPERADOR && t_char() == FECHA_PARENTESE)) error(8);
        get_next();
    }
    return false;
}

bool procedure(){
    if(t.cat == RESERVED && t_char() == PROC){
        get_next();
        if(!identifier()) error(10);
        //push name proc
        get_next();
        if(!(t.cat == OPERADOR && t_char() == ABRE_PARENTESE)) error(7);
        get_next();
        param();
        if(!(t.cat == OPERADOR && t_char() == FECHA_PARENTESE)) error(8);
        get_next();
        while(declaration()){
            //push
            while(get_next() && t.cat == OPERADOR && t_char() == VIRGULA){
                get_next();
                if(!identifier()) error(5);
                //push
            }
        }
        while(cmd());
        if(!(t.cat == RESERVED && t_char() == ENDPROC)) error(11);
        get_next();
        return true;
    }
    else if(t.cat == RESERVED && t_char() == FWD){
        get_next();
        if(t.cat == RESERVED && t_char() == PROC) error(12);
        get_next();
        if(!identifier()) error(10);
        get_next();
        if(!(t.cat == OPERADOR && t_char() == ABRE_PARENTESE)) error(7);
        get_next();
        if(!type) error(13);
        while(get_next() && t.cat == OPERADOR && t_char() == VIRGULA){
            get_next();
            if(!type) error(14);
        }
        if(!(t.cat == OPERADOR && t_char() == FECHA_PARENTESE)) error(8);
        get_next();
        return true;
    }
    return false;
}

bool factor(){
    if(ct_car() || ct_int() || ct_real()){
        return true;
    }
    else if(identifier()){
        get_next();
        if(!(t.cat == OPERADOR && t_char() == ABRE_PARENTESE)) return true;
        get_next();
        if(!expr()) error(25);
        while(get_next() && t.cat == OPERADOR && t_char() == VIRGULA) if(!expr()) error(26);
        if(!(t.cat == OPERADOR && t_char() == FECHA_PARENTESE)) error(8);
        return true;
    }
    else if(t.cat == OPERADOR && t_char() == ABRE_PARENTESE){
        get_next();
        if(!expr()) error(25);
        if(!(t.cat == OPERADOR && t_char() == FECHA_PARENTESE)) error(8);
        return true;
    }
    else{
        if(!(t.cat == LOGICO && t_char() == NOT)) return false;
        get_next();
        return factor();
    }
}

bool term(){
    if(!factor()) return false;
    get_next();
    if((t.cat == OPERADOR && (t_char() == VEZES || t_char() == DIVISAO)) || 
        (t.cat == LOGICO && t_char() == NOT)){
        if(!factor()) error(27);
        get_next();
    }
    return true;
}

bool simple_expr(){
    if(t.cat == OPERADOR && (t_char() == MAIS || t_char() == MENOS)){
        get_next();
        if(!term()) error(28);
        printf("\ngloria a deuxx\n");
    }
    else if(!term()) {
        printf("\ngloria a deuxxx\n");
        return false;
    }
    printf("\ngloria a deuxxx\n");
    while((t.cat == OPERADOR && (t_char() == MAIS || t_char() == MENOS)) || (t.cat == LOGICO
        && t_char() == OR)){
        if(!term()) error(29);
    }
    return true;
}

bool op_rel(){
    return t.cat == LOGICO && t_char() < 5;
}

bool expr(){
    if(!simple_expr()) return false;
    if(op_rel()) if(!simple_expr()) error(30);
    return true;
}

bool attribution(){
    if(!identifier()) return false;
    get_next();
    if(!(t.cat == OPERADOR && t_char() == IGUAL)) error(23);
    if(!expr()) error(24);
    return true;
}

bool cmd(){
    switch(t.cat){
        case RESERVED:
            switch(t_char()){
                case IF:
                    get_next();
                    if(!(t.cat == OPERADOR && t_char() == ABRE_PARENTESE)) error(7);
                    get_next();
                    expr();
                    if(!(t.cat == OPERADOR && t_char() == FECHA_PARENTESE)) error(8);
                    get_next();
                    cmd();
                    while(cmd());
                    if(t.cat == RESERVED && t_char() == ELSE){
                        get_next();
                        cmd();
                        while(cmd());
                    }
                    if(!(t.cat == RESERVED && t_char() == ENDIF)) error(16);
                    get_next();
                    return true;
                case WHILE:
                    get_next();
                    if(!(t.cat == OPERADOR && t_char() == ABRE_PARENTESE)) error(7);
                    get_next();
                    expr();
                    if(!(t.cat == OPERADOR && t_char() == FECHA_PARENTESE)) error(8);
                    get_next();
                    cmd();
                    while(cmd());
                    if(!(t.cat == RESERVED && t_char() == ENDWHILE)) error(17);
                    get_next();
                    return true;
                case FOR:
                    break;
                case RETURN:
                    get_next();
                    if(!(t.cat == OPERADOR && t_char() == ABRE_PARENTESE)) error(7);
                    get_next();
                    expr();
                    if(!(t.cat == OPERADOR && t_char() == FECHA_PARENTESE)) error(8);
                    get_next();
                    return true;
                case CALL:
                    get_next();
                    if(!identifier()) error(18);
                    if(!(t.cat == OPERADOR && t_char() == ABRE_PARENTESE)) error(7);
                    get_next();
                    if(expr()){
                        while(get_next() && t.cat == OPERADOR && t_char() == VIRGULA) if(!expr()) error(19);
                    }
                    if(!(t.cat == OPERADOR && t_char() == FECHA_PARENTESE)) error(8);
                    get_next();
                    return true;
                case KEYBOARD:
                    get_next();
                    if(!identifier()) error(21);
                    while(get_next() && t.cat == OPERADOR && t_char() == VIRGULA) if(!identifier()) error(21);
                case DISPLAY:
                    get_next();
                    if(!(identifier() || ct_int() || ct_car() || ct_real() || ct_string())) error(20);
                    get_next();
                    if(t.cat == DUP){
                        get_next();
                        if(!(identifier() || ct_int())) error(22);
                    }
                    while(get_next() && t.cat == OPERADOR && t_char() == VIRGULA){
                        get_next();
                        if(!(identifier() || ct_int() || ct_car() || ct_real() || ct_string())) error(20);
                        get_next();
                        if(t.cat == DUP){
                            get_next();
                            if(!(identifier() || ct_int())) error(22);
                        }
                    }
                    get_next();
                    return true;
                default:
                    return false;
            }
            break;
        case IDENTIFIER:
            get_next();
            return attribution();
        case OPERADOR:
            return t_char() == PONTO_VIRGULA;
    }
}

void prog(){
    if(t.cat == RESERVED && t_char() == PROG){
        get_next();
        cmd();
        while(cmd());
        if(!(t.cat == RESERVED && t_char() == ENDPROG)) error(2);
    }
}

void start(){
    get_next();
    if(t.cat == RESERVED && t_char() == PL){
        get_next();
        if(t.cat == IDENTIFIER){
            get_next();
            var();
            while(function() || procedure());
            prog();
        }
        else error(1);
    }
    else error(0);
}

void analise(){
    get_next();
    if(expr()) printf("Gloria... Gloria Deux\nExpressao valida\n");
}