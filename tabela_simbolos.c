#include "tabela_simbolos.h"
#include "sintatico.h"
#include "error.h"

// FUNCOES DO MYVECTOR

bool cmp_symbol(symbol x, symbol y){
    return (strcmp(x.nome, y.nome) && x.scopo == y.scopo && 
        x.categoria == y.categoria && x.tipo == y.tipo);
}

void free_symbol(symbol i){
    if(i.nome) free(i.nome);
}

symbol *new_symbol(char* nome, int categoria, int tipo, int scopo){
    symbol *i;
    i = malloc(sizeof(symbol));
    if(!i) execution_erro(1);
    i->nome = NULL;
    if(nome){
        i->nome = (char*) malloc(strlen(nome) + 1);
        if(!i->nome) execution_erro(1);
        strcpy(i->nome, nome);
    }
    i->categoria = categoria;
    i->tipo = tipo;
    i->scopo = scopo;
    return i;
}

void free_table(){
    for(int i=get_table()->current; i>=0; i--) 
        if(get_table()->value[i]) free_symbol(*symbol_at(i));
    free_vector(get_table());
}

void push_back_symbol(symbol *value){
    push_back(get_table(), value);
}


symbol *symbol_at(int p){
    symbol *s = value_at(get_table(), p);
    return s;
}

void pop_until_param(){
    for(int i=get_table()->current; i>=0 && symbol_at(i) && (symbol_at(i)->categoria != PARAM) && 
        (symbol_at(i)->scopo == LOCAL); i--)
        pop_back(get_table());
}

void alter_symbol_at(int p, symbol *i){
    alter_at(get_table(), p, i);
}

int find_value_by_name(char* s, int position){
    if(position != -1){
        int p = find_value_by_name_local(s, position);
        if(p != -1) return p;
    }
    return find_no_param_by_name(s);
}

int find_no_param_by_name(char* s){
    for(int i=get_table()->current; i>=0; i--)
        if(symbol_at(i) && symbol_at(i)->nome && strcmp(symbol_at(i)->nome, s) == 0 && 
        symbol_at(i)->categoria != PARAM) return i;
    return -1;
}

int find_param_by_name(char* s, int position){
    if(position != -1){
        for(int i = position+1; i<get_table()->current; i++){
            if(symbol_at(i) && symbol_at(i)->categoria == PARAM){
                if(symbol_at(i)->nome && strcmp(symbol_at(i)->nome, s) == 0) return i;
            }
            else break;
        }
    }
    return -1;
}

int find_function_procudure(){
    for(int i=get_table()->current; i>=0; i--)
        if(symbol_at(i) && 
        (symbol_at(i)->categoria == FUNCTION || 
        symbol_at(i)->categoria == FUNCTION_SIGNATURE ||
        symbol_at(i)->categoria == PROCEDURE ||
        symbol_at(i)->categoria == PROCEDURE_SIGNATURE)
        ) return i;
    return -1;
}

int find_function_procudure_signature(){
    for(int i=get_table()->current; i>=0; i--)
        if(symbol_at(i) && 
        (symbol_at(i)->categoria == FUNCTION_SIGNATURE ||
        symbol_at(i)->categoria == PROCEDURE_SIGNATURE)
        ) return i;
    return -1;
}

int find_function_by_name(char* s){
    for(int i=get_table()->current; i>=0; i--)
        if(symbol_at(i) && symbol_at(i)->nome && strcmp(symbol_at(i)->nome, s) == 0){
            if(symbol_at(i)->categoria == FUNCTION || symbol_at(i)->categoria == FUNCTION_SIGNATURE) return i;
            break;
        }
    return -1;
}

int find_procedure_by_name(char* s){
    for(int i=get_table()->current; i>=0; i--)
        if(symbol_at(i) && symbol_at(i)->nome && strcmp(symbol_at(i)->nome, s) == 0){
            if(symbol_at(i)->categoria == PROCEDURE || symbol_at(i)->categoria == PROCEDURE_SIGNATURE) return i;
            break;
        }
    return -1;
}

int find_variable_by_name(char* s){
    for(int i=get_table()->current; i>=0; i--) 
        if(symbol_at(i)->nome && strcmp(symbol_at(i)->nome, s) == 0){
            if(symbol_at(i)->categoria == VARIAVEL) return i;
            break;
        }
    return -1;
}

int find_value_by_name_local(char* s, int position){
    for(int i=get_table()->current; i>=0; i--)
        if(symbol_at(i) && symbol_at(i)->categoria != PARAM && symbol_at(i)->scopo == LOCAL){
            if(symbol_at(i)->nome && strcmp(symbol_at(i)->nome, s) == 0) return i;
        }
        else break;
    return find_param_by_name(s, position);
}

int find_until_function_by_name(char* s){
    for(int i=get_table()->current; i>=0 && symbol_at(i) && symbol_at(i)->categoria != FUNCTION &&
        symbol_at(i)->categoria != PROCEDURE && 
        symbol_at(i)->categoria != FUNCTION_SIGNATURE &&
        symbol_at(i)->categoria != PROCEDURE_SIGNATURE; i--)
        if(symbol_at(i)->nome && strcmp(symbol_at(i)->nome, s) == 0) return i;
    return -1;
}

void print_table(){
    for(int i=0; i<=get_table()->current; i++) 
        if(symbol_at(i))
            printf("| Nome : %s | Tipo: %i | Categoria: %i | Scopo: %i|\n", 
                symbol_at(i)->nome ? symbol_at(i)->nome : "",
                symbol_at(i)->tipo,
                symbol_at(i)->categoria,
                symbol_at(i)->scopo);
}
