#include "sintatico.h"

void analise(){
    token t;
    while(has_next(t = get_token())) print_token(t);
}