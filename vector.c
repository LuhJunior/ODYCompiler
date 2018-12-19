#include "vector.h"

myvector *new_vector(int tam){
    myvector *vec;
    vec = malloc(sizeof(myvector));
    if(!vec) err("Erro de alocacao");
    vec->value = (void **) malloc(sizeof(void *) * (tam + 10));
    if(!vec->value) err("Erro de alocacao");
    vec->tam = tam + 10;
    vec->current = - 1;
    return vec;
}

void push_back(myvector *vec, void* value){
    if(vec->current == vec->tam-1){
        vec->tam += 10;
        void *values = malloc(sizeof(void *) * vec->tam);
        if(!values) err("Erro de alocacao");
        memcpy(values, vec->value, sizeof(void *) * (vec->current+1));
        free(vec->value);
        vec->value = values;
    }
    vec->value[++vec->current] = value;
}

void pop_back(myvector *vec){
    if(vec->current >= 0) free(vec->value[vec->current--]);
}

void *value_at(myvector *vec, int p){
    if(p <= vec->current && p>=0) return(vec->value[p]);
    printf("out of range: %i\n", p);
    return NULL;
}

void alter_at(myvector *vec, int p, void *i){
    if(p <= vec->current && p>=0){
        free(vec->value[p]);
        vec->value[p] = i;
    }
    else printf("out of range: %i\n", p);
}

void free_vector(myvector *vec){
    for(int i=vec->current; i>=0; i--) free(vec->value[i]);
    if(vec->value) free(vec->value);
}

void err(char *s){
    printf("%s\n", s);
    exit(0);
}
/*
int main(int argc, char const *argv[])
{
    myvector *v = new_vector(10);
    int a = 6;
    push_back(v, &a);
    printf("%i\n", *(int *)value_at(v, 0));
    return 0;
}
*/
