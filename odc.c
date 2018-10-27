#include <stdio.h>
#include "sintatico.h"

int main(int argc, char **argv){
    token t;
    if(argc <= 1){
        printf("Error: nenhum arquivo passado\n");
        return 0;
    }
    if(prepare_file(argv[1])){
        analise();
        close_file();
        printf("Fim da Analise");
    }
    else{
        printf("Erro ao abrir o arquivo ou arquivo nao existe!\n");
    }
    return 0;
}