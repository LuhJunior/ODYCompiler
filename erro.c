#include "erro.h"
#include "sintatico.h"

char ErrosSintaticos[][70] = {"Palavra reservada de inicializacao faltando", "Nome do programa faltando", "endprog faltando",
    "declaracao de variavel incorreta", "endvar faltando", "identificador depois do tipo faltando",
    "identificador depois do tipo da funcao faltando", "Faltando abre parenteses", "Faltando fecha parenteses",
    "Faltando endfunc", "identificador do procedimento faltando", "Faltando endproc",
    "Faltando proc depois de fwd", "Faltando tipo ou noparam depois do abre parenteses do procedimento", "Faltando tipo depois da virgula",
    "endif faltando", "endwhile faltando", "Faltando identificador depois de instrucao call",
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
    "identificador invalido", "O retorno deve ser do mesmo tipo da funcao"
};

char ErrosSistema[][70] = {"Funcao ou procedimento nao encontrada na tabela"};

char ErrosLexicos[][50] = {"E esperado um digito, a, o, ou n depois do ponto", "E esperado um caracter imprimivel",
    "Ao fim de um \".and\", \".or\" ou \".not\" espera-se \".\"", "E esperado um \" \' \" depois de um caracter valido",
    "Era esperado um n depois do \"a\"", "Era esperado um a, o, ou n depois do ponto", "Era esperado \"r\" depois do \"o\"",
    "Era esperado \"o\" depois do \"n\"", "Era esperado \"d\" depois do \"n\"", "Era esperado \"t\" depois do \"o\"",
    "Caracter invalido"};

void error(int e){
    printf("\nErro Sintatico %i na linha %i e na coluna %i: %s\n", e, get_line(), get_column(), ErrosSintaticos[e]);
    exit(1);
}

void error2(int e){
    printf("\nErro Semantico %i na linha %i e na coluna %i: %s\n", e, get_line(), get_column(), ErrosSemanticos[e]);
    exit(1);
}

void execution_erro(int e){
    printf("\nErro de Execucao %i na linha %i e na coluna %i: %s\n", e, get_line(), get_column(), ErrosSistema[e]);
    exit(1);
}

void erro_lexico(int e){
    printf("\nErro Lexico %i na linha %i e na coluna %i: %s\n", e, get_line(), get_column(), ErrosLexicos[e]);
    exit(1);
}