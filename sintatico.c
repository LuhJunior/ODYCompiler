#include "sintatico.h"

token t;
Erros[][20] = {"Palavra reservada de inicializacao faltando", "Nome do programa faltando", "endprog faltando",
                "declaracao de variavel incorreta", "endvar faltando", "identificador depois do tipo faltando",
                "identificador depois do tipo da funcao faltando", "Faltando abre parenteses", "Faltando fecha parenteses",
                "Faltando endfunc", "identificador do procedimento faltando", "Faltando endproc", "Faltando proc depois de fwd",
                "Faltando tipo depois do abre parenteses do procedimento", "Faltando tipo depois da virgula",
                };


bool cmp_item(item i1, item i2){
    return true;
}

void free_item(item i){

}

myvector new_vector(void *value, int tam){
    myvector v;
    v.value = (item *) malloc(sizeof(item) * tam + 10);
    v.tam = tam + 10;
    v.current = 0;
    for(int i=0; i<tam && value != NULL; i++) v.value[i] = value;
    return v;
}

void free_vector(vector v){
    for(int i=0; i<v.current; i++) free_item(v.value[i]);
    free(v.valor);
}

void push_back(myvector v, void *value){
    if(v.current <= v.tam){
        v.tam += 10;
        item *values = valor = (item *) malloc(sizeof(item) * v.tam);
        for(int i=0; i<v.current; i++) values[i] = v.value[i];
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
item *find_value(myvector v, item i){
    for(int i=0; i<v.current; i++) if(cmp_item(v.value[i], i)) return v.value + i;
    return NULL;
}

void error(int e){
    printf("Erro Sintatico: %s", Erros[e]);
}

bool get_next(){
    free_token(t);
    t = get_token();
    return true;
}

bool type(){
    return (t.cat == RESERVED && (t.cat == CHAR || t.cat == INT || t.cat == REAL
            || t.cat == BOOL));
}

bool identifier(){
    return (t.cat == IDENTIFIER);
}

void var(){
    while(type()){
        get_next();
        if(identifier()){
            //push var
            while(get_next() && t.cat == OPERADOR && *cchar(t.value) == VIRGULA){
                get_next();
                if(!identifier()) error(3);
            }
            if(!(t.cat == RESERVED && *cchar(t.value) == ENDVAR)) error(4);
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
        while(get_next() && t.cat == OPERADOR && *cchar(t.value) == VIRGULA){
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
        if(!(t.cat == OPERADOR && *cchar(t.value) == ABRE_PARENTESE)) error(7)
        get_next();
        param();
        if(!(t.cat == OPERADOR && *cchar(t.value) == FECHA_PARENTESE)) error(8)
        get_next();
        while(declaration()){
            //push
            while(get_next() && t.cat == OPERADOR && *cchar(t.value) == VIRGULA){
                get_next();
                if(!identifier()) error(5);
                //push
            }
        }
        while(cmd());
        if(!(t.cat == RESERVED && *cchar(t.value) == ENDFUNC)) error(9);
        get_next();
        return true;
    }
    return false;
}

bool procedure(){
    if(t.cat == RESERVED && *cchar(t.value) == PROC){
        get_next();
        if(!identifier()) error(10);
        //push name proc
        get_next();
        if(!(t.cat == OPERADOR && *cchar(t.value) == ABRE_PARENTESE)) error(7)
        get_next();
        param();
        if(!(t.cat == OPERADOR && *cchar(t.value) == FECHA_PARENTESE)) error(8)
        get_next();
        while(declaration()){
            //push
            while(get_next() && t.cat == OPERADOR && *cchar(t.value) == VIRGULA){
                get_next();
                if(!identifier()) error(5);
                //push
            }
        }
        while(cmd());
        if(!(t.cat == RESERVED && *cchar(t.value) == ENDPROC)) error(11);
        get_next();
        return true;
    }
    else if(t.cat == RESERVED && *cchar(t.value) == FWD){
        get_next();
        if(t.cat == RESERVED && *cchar(t.value) == PROC) error(12);
        get_next();
        if(!identifier()) error(10);
        get_next();
        if(!(t.cat == OPERADOR && *cchar(t.value) == ABRE_PARENTESE)) error(7);
        get_next();
        if(!type) error(13);
        while(get_next() && t.cat == OPERADOR && *cchar(t.value) == VIRGULA){
            get_next();
            if(!type) error(14);
        }
        if(!(t.cat == OPERADOR && *cchar(t.value) == FECHA_PARENTESE)) error(8);
        get_next();
        return true;
    }
    return false;
}

bool expr(){

}

bool cmd(){
    switch(t.cat){
        case RESERVED:
            switch(*cchar(t.value)){
                case IF:
                    get_next();
                    if(!(t.cat == OPERADOR && *cchar(t.value) == ABRE_PARENTESE)) error(7)
                    get_next();
                    expr();
                    if(!(t.cat == OPERADOR && *cchar(t.value) == FECHA_PARENTESE)) error(8);
                    get_next();
                    cmd();
                    while(cmd());
                    if(t.cat == RESERVED && *cchar(t.value) == ELSE){
                        get_next();
                        cmd();
                        while(cmd());
                    }
                    if(!(t.cat == RESERVED && *cchar(t.value) == ENDIF)) error();
                    get_next();
                    return true;
                case WHILE:
                    break;
                case FOR:
                    break;
                case RETURN:
                    break;
                case CALL:
                    break;
                case DISPLAY:
                    break;
            }
            break;
        case IDENTIFIER:
            break;
        case OPERADOR:
            break;
    }
}

void prog(){
    if(t.cat == RESERVED && *cchar(t.value) == PROG){
        get_next();
        cmd();
        while(cmd());
        if(!(t.cat == RESERVED && *cchar(t.value) == ENDPROG)) error(2);
    }
}

void start(){
    get_next();
    if(t.cat == RESERVED && *cchar(t.value) == PL){
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
    
}