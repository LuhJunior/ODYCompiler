#include "sintatico.h"
#include "error.h"

token t, t_ahead;
int line, column;
myvector *table;
char expression_type;
char inside_function = -1;


//GET TABLE
myvector *get_table(){
    return table;
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

void check_identifier(symbol *ident){
    int position = -1;
    switch(ident->categoria){
        case VARIAVEL:
            if(ident->scopo == GLOBAL) position = find_no_param_by_name(ident->nome);
            else position = find_value_by_name_local(ident->nome, inside_function);
            if(position != -1) error2(0);
            break;
        case FUNCTION_SIGNATURE:
            position = find_no_param_by_name(ident->nome);
            if(position != -1) error2(0);
            break;
        case PROCEDURE_SIGNATURE:
            position = find_no_param_by_name(ident->nome);
            if(position != -1) error2(0);
            break;
        case FUNCTION:
            position = find_no_param_by_name(ident->nome);
            inside_function = position;
            if(position != -1){
                if(!(symbol_at(position)->categoria == FUNCTION_SIGNATURE)) error2(0);
                if(!(symbol_at(position)->tipo == ident->tipo)) error2(1);
                param(position);
                alter_symbol_at(position, ident);
            }
            else{
                push_back_symbol(ident);
                inside_function = table->current;
                param(-1);
            }
            break;
        case PROCEDURE:
            position = find_no_param_by_name(ident->nome);
            inside_function = position;
            if(position != -1){
                if(!(symbol_at(position)->categoria == PROCEDURE_SIGNATURE)) error2(0);
                param(position);
                alter_symbol_at(position, ident);
            }
            else{
                push_back_symbol(ident);
                inside_function = table->current;
                param(-1);
            }
            break;
        case PARAM:
            position = find_value_by_name_local(ident->nome, inside_function);
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
        if((op == S_MAIS) || (op == S_MENOS) || (op == S_VEZES) || (op == S_DIVISAO)) return new_expression(INT, NULL);
        else if((op == S_OR) || (op == S_AND) || (op == S_MAIOR) || (op == S_MENOR) ||
            (op == S_MAIORIGUAL) || (op == S_MENORIGUAL) || (op == S_IGUAL) || (op ==S_HASHTAG)) return new_expression(BOOL, NULL);
    }
    else if((e.type == REAL && e2.type == INT) || (e.type == INT && e2.type == REAL) ||
        (e.type == REAL && e2.type == BOOL) || (e.type == BOOL && e2.type == INT)){
        //Apenas um extra para fazer mais sentido. Nao faz parte da especificacao
        if((op == S_MAIS) || (op == S_MENOS) || (op == S_VEZES) || (op == S_DIVISAO)) return new_expression(REAL, NULL);
        else if((op == S_OR) || (op == S_AND) || (op == S_MAIOR) || (op == S_MENOR) ||
            (op == S_MAIORIGUAL) || (op == S_MENORIGUAL) || (op == S_IGUAL) || (op ==S_HASHTAG)) return new_expression(BOOL, NULL);
    }
    else if(e.type == e2.type){
        if(op == S_MAIS || op == S_MENOS || op == S_VEZES || op == S_DIVISAO) return new_expression(e.type, NULL);
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
    if((tipo == REAL && tipo2 != REAL) || (tipo != REAL && tipo2 == REAL) || (tipo == BOOL && tipo2 == INT) ) error2(9);
}

void validar_funcao(int position){
    if(t_ahead.cat == IDENTIFIER){
        get_next();
        int i = find_value_by_name(t_string(), inside_function);
        if(i == -1) error2(4);
        if(symbol_at(++position)->categoria != PARAM) error2(6);
        if(symbol_at(position)->tipo != symbol_at(i)->tipo) error2(5);
        while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
            get_next();
            get_next();
            if(!identifier()) error(25);
            i = find_value_by_name(t_string(), inside_function);
            if(i == -1) error2(4);
            if(symbol_at(++position)->categoria != PARAM) error2(6);
            if(symbol_at(position)->tipo != symbol_at(i)->tipo) error2(5);
        }
    }
}

// FUNCOES SINTATICAS

void var(){
    symbol *i = NULL;
    while(type()){
        int tipo = t_char();
        get_next();
        if(!identifier()) error(5);
        i = new_symbol(t_string(), VARIAVEL, tipo, GLOBAL);
        check_identifier(i);
        push_back_symbol(i);
        while(get_next() && operator() == VIRGULA){
            get_next();
            if(!identifier()) error(3);
            i = new_symbol(t_string(), VARIAVEL, tipo, GLOBAL);
            check_identifier(i);
            push_back_symbol(i);
        }
    }
    if(!(reserved() == ENDVAR)) error(4);
}

bool declaration(){
    symbol *i = NULL;
    if(!type_ahead()) return false;
    get_next();
    int tipo = t_char();
    get_next();
    if(!identifier()) error(5);
    i = new_symbol(t_string(), VARIAVEL, tipo, LOCAL);
    check_identifier(i);//olha na tabela de simbolos
    push_back_symbol(i);
    while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
        get_next();
        get_next();
        if(!identifier()) error(5);
        i = new_symbol(t_string(), VARIAVEL, tipo, LOCAL);
        check_identifier(i);
        push_back_symbol(i);
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
            symbol *i = NULL;
            get_next();
            if(!identifier()) error(5);
            i = new_symbol(t_string(), PARAM, tipo, LOCAL);
            check_identifier(i);
            push_back_symbol(i);
            while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
                get_next();
                get_next();
                if(!type()) error(14);
                tipo = t_char();
                get_next();
                if(!identifier()) error(5);
                i = new_symbol(t_string(), PARAM, tipo, LOCAL);
                check_identifier(i);
                push_back_symbol(i);
            }
        }
    }
    else{
        if(type_ahead()){
            get_next();
            int tipo = t_char();
            symbol *i;
            get_next();
            if(!identifier()) error(5);
            i = new_symbol(t_string(), PARAM, tipo, LOCAL);
            check_identifier(i);
            if(symbol_at(++position)->categoria != PARAM) error2(2);
            if(symbol_at(position)->tipo != i->tipo) error2(3);
            alter_symbol_at(position, i);
            while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
                get_next();
                get_next();
                if(!type()) error(18);
                tipo = t_char();
                get_next();
                if(!identifier()) error(5);
                i = new_symbol(t_string(), PARAM, tipo, LOCAL);
                check_identifier(i);
                if(symbol_at(++position)->categoria != PARAM) error2(2);
                if(symbol_at(position)->tipo != i->tipo) error2(3);
                alter_symbol_at(position, i);
            }
        }
        else if(symbol_at(++position)->categoria == PARAM) error2(2);
    }
    get_next();
    if(!(operator() == FECHA_PARENTESE)) error(8);
    return true;
}

bool function(){
    if(type()){
        symbol *ident = NULL;
        int tipo = t_char();
        get_next();
        if(!identifier()) error(6);
        ident = new_symbol(t_string(), FUNCTION, tipo, GLOBAL);
        check_identifier(ident);
        while(declaration());
        get_next();
        while(cmd() && get_next());
        pop_until_param();
        if(!(reserved() == ENDFUNC)) error(9);
        inside_function = -1;
        return true;
    }
    else if(reserved() == FWD){
        if(t_ahead.cat == RESERVED && t2_char() == PROC) return procedure();
        get_next();
        if(!type()) error(30);
        int tipo = t_char();
        symbol *i = NULL;
        get_next();
        if(!identifier()) error(6);
        i = new_symbol(t_string(), FUNCTION_SIGNATURE, tipo, GLOBAL);
        check_identifier(i);
        push_back_symbol(i);
        get_next();
        if(!(operator() == ABRE_PARENTESE)) error(7);
        get_next();
        if(type()){
            int tipo = t_char();
            symbol *i = new_symbol(NULL, PARAM, tipo, LOCAL);
            push_back_symbol(i);
            while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
                get_next();
                get_next();
                if(!type()) error(13);
                i = new_symbol(NULL, PARAM, tipo, LOCAL);
                push_back_symbol(i);
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
    symbol *ident = NULL;
    if(reserved() == PROC){
        get_next();
        if(!identifier()) error(10);
        ident = new_symbol(t_string(), PROCEDURE, -1, GLOBAL);
        check_identifier(ident);
        while(declaration());
        get_next();
        while(cmd() && get_next());
        pop_until_param();
        if(!(reserved() == ENDPROC)) error(11);
        inside_function = -1;
        return true;
    }
    else if(reserved() == FWD){
        get_next();
        if(reserved() != PROC) error(12);
        get_next();
        if(!identifier()) error(10);
        ident = new_symbol(t_string(), PROCEDURE_SIGNATURE, -1, GLOBAL);
        check_identifier(ident);
        push_back_symbol(ident);
        get_next();
        if(operator() != ABRE_PARENTESE) error(7);
        get_next();
        if(type()){
            ident = new_symbol(NULL, PARAM, t_char(), LOCAL);
            push_back_symbol(ident);
            while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
                get_next();
                get_next();
                if(!type()) error(14);
                ident = new_symbol(NULL, PARAM, t_char(), LOCAL);
                push_back_symbol(ident);
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
        int position = find_value_by_name(t_string(), inside_function);
        if(position == -1) error2(4);
        e = new_expression(symbol_at(position)->tipo, NULL);
        if(!(t_ahead.cat == OPERADOR && t2_char() == ABRE_PARENTESE)) return e;
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
        if(!(operator() == FECHA_PARENTESE)) error(8);
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
    while((t_ahead.cat == OPERADOR && (t2_char() == VEZES || t2_char() == DIVISAO)) || 
        (t_ahead.cat == LOGICO && t2_char() == AND)){
        char op = expression_op();
        if(op == -1) error2(44);
        get_next();
        get_next();
        e2 = factor();
        if(e2.type == -1) error(25);
        e = comp_expression(e, op, e2);
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
    position = find_value_by_name(t_string(), inside_function);
    if(position == -1) error2(11);
    get_next();
    if(!(operator() == ATRIBUICAO)) error(23);
    get_next();
    e = expr();
    if(e.type == -1) error(24);
    comp_atrib(symbol_at(position)->tipo, e.type);
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
                    if(inside_function != -1) if(symbol_at(inside_function)->categoria == PROCEDURE) return true;
                    get_next();
                    if(!(operator() == ABRE_PARENTESE)) error(7);
                    get_next();
                    e = expr();
                    if(e.type == -1) error(23);
                    get_next();
                    if(!(operator() == FECHA_PARENTESE)) error(8);
                    if(inside_function != -1) if(symbol_at(inside_function)->tipo != e.type) error2(15);
                    return true;
                case CALL:
                    position = -1;
                    get_next();
                    if(!identifier()) error(18);
                    position = find_value_by_name(t_string(), inside_function);
                    if(position == -1) error2(4);
                    if(symbol_at(position)->categoria != PROCEDURE &&
                        symbol_at(position)->categoria != PROCEDURE_SIGNATURE) error2(10);
                    if(!(t_ahead.cat == OPERADOR && t2_char() == ABRE_PARENTESE)) error(7);
                    get_next();
                    validar_funcao(position);
                    get_next();
                    if(!(operator() == FECHA_PARENTESE)) error(8);
                    return true;
                case KEYBOARD:
                    position = -1;
                    get_next();
                    if(!identifier()) error(21);
                    position = find_value_by_name(t_string(), inside_function);
                    if(position == -1) error2(11);
                    if(symbol_at(position)->categoria != VARIAVEL) error2(12);
                    while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
                        get_next();
                        get_next();
                        if(!identifier()) error(21);
                        position = find_value_by_name(t_string(), inside_function);
                        if(position == -1) error2(11);
                        if(symbol_at(position)->categoria != VARIAVEL) error2(12);
                    }
                    return true;
                case DISPLAY:
                    position = -1;
                    get_next();
                    if(!(identifier() || ct_int() || ct_car() || ct_real() || ct_string())) error(20);
                    if(identifier()){
                        position = find_value_by_name(t_string(), inside_function);
                        if(position == -1) error2(11);
                        if(symbol_at(position)->categoria != VARIAVEL 
                            && symbol_at(position)->categoria != PARAM) error2(13);
                    }
                    //get_next();
                    if(t_ahead.cat == RESERVED && t2_char() == DUP){
                        get_next();
                        get_next();
                        if(!(identifier() || ct_int())) error(22);
                        if(identifier()){
                            position = find_value_by_name(t_string(), inside_function);
                            if(position == -1) error2(11);
                            if(symbol_at(position)->categoria != VARIAVEL 
                                && symbol_at(position)->categoria != PARAM) error2(13);
                        }
                    }
                    while(t_ahead.cat == OPERADOR && t2_char() == VIRGULA){
                        get_next();
                        get_next();
                        if(!(identifier() || ct_int() || ct_car() || ct_real() || ct_string())) error(20);
                        if(identifier()){
                            position = find_value_by_name(t_string(), inside_function);
                            if(position == -1) error2(11);
                            if(symbol_at(position)->categoria != VARIAVEL 
                                && symbol_at(position)->categoria != PARAM) error2(13);
                        }
                        if(t_ahead.cat == RESERVED && t2_char() == DUP){
                            get_next();
                            get_next();
                            if(!(identifier() || ct_int())) error(22);
                            if(identifier()){
                                position = find_value_by_name(t_string(), inside_function);
                                if(position == -1) error2(11);
                                if(symbol_at(position)->categoria != VARIAVEL 
                                    && symbol_at(position)->categoria != PARAM) error2(13);
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
    table = new_vector(0);
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
        if(find_function_procudure_signature() != -1) error2(16);
        inside_function = -1;
        prog();
        if(has_next(t_ahead)) error(45);
    }
    else error(0);
}

void analise(){
    start();
    printf("Programa valido\n");
    free_table();
}