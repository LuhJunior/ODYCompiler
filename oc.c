#include <stdio.h>
#include "lexico.h"

int main(){
    token t;
    FILE *in;
    in = fopen("entrada.txt", "r");
    if(in != NULL){
        while(!feof(in)){
            t = analise(in);
            print_token(t);
        }
        fclose(in);
        printf("Fim da Analise");
    }
    else{
        printf("Erro ao abrir o arquivo ou arquivo nao existe!\n");
    }
    return 0;
}