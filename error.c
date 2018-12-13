#include "error.h"
#include "sintatico.h"

char ErrosSintaticos[][80] = {"1. Palavra reservada de inicializacao faltando", "2. Nome do programa faltando", "3. endprog faltando",
    "4. Declaracao de variavel incorreta", "5. endvar faltando", "6. Identificador depois do tipo faltando",
    "7. Identificador depois do tipo da funcao faltando", "8. Faltando abre parenteses", "9. Faltando fecha parenteses",
    "10. Faltando endfunc", "11. Identificador do procedimento faltando", "12. Faltando endproc",
    "13. Faltando proc depois de fwd", "14. Faltando tipo ou noparam depois do abre parenteses do procedimento", "15. Faltando tipo depois da virgula",
    "16. endif faltando", "17. endwhile faltando", "18. Faltando identificador depois de instrucao call",
    "19. Depois da virgula tem que vir outra expressao na instrucao call", "20. Constante ou identificador faltando depois da instrucao display",
    "21. Identificador faltando depois da instrucao keyboard", "22. Identificador ou constante inteira faltando depois da instrucao dup",
    "23. Operador igual depois do identificador faltando", "24. Expressao depois do identificador igual faltando ou invalida",
    "25. Expressao faltando depois de abre parentesis", "26. Expressao faltando depois da virgula", 
    "27. Fator depois de operador faltando", "28. Termo faltando depois de operador", "29. Faltando termo depois do operador",
    "30. Faltando expressao simples depois do operador relacional", "31. Faltando o tipo depois do fwd", "32. prog faltando",
    "33. Faltou atribuicao", "34. Faltou virgula", "35. Faltou endfor", "36. Deve haver um comando depois da instrucao  if", "37. Deve haver um comando depois da instrucao  else", 
    "38. Deve haver um comando depois da instrucao  while", "39. Deve haver um comando depois da instrucao for",
    "40. Deve haver um comando depois da instrucao prog", "41. Faltando expressao depois do abre parentesis",
    "42. Faltando expressao depois da virgula", "43. faltando expressao depois do igual", "44. Expressao invalida",
    "45. Operador invalido", "46. Faltando fator depois do NOT"
};

char ErrosSemanticos[][70] = {"Esse Nome de identificador ja esta em uso", "O tipo da funcao declarada nao e igual ao da assinatura",
    "A quantidade de parametro(s) e diferente da assinatura", "Tipo do parametro diferente da assinatura",
    "Variavel, funcao ou procedimento nao foi declarado", "Argumento nao compativel com a funcao", 
    "Quantidade invalida de argumentos", "tipos da expressao incompativeis", "A expressao nao e um bool",
    "atribuicao invalida", "A instrucao call so pode chamar um procedimento", "Variavel nao foi declarada",
    "a instrucao keyborad espera uma variavel", "o(s) identificador(es) da instrucao display deve(m) ser uma variavel",
    "identificador invalido", "O retorno deve ser do mesmo tipo da funcao"
};

char ErrosSistema[][70] = {"Funcao ou procedimento nao encontrada na tabela", "Falha na alocacao de memoria"};

char ErrosLexicos[][50] = {"E esperado um digito, a, o, ou n depois do ponto", "E esperado um caracter imprimivel",
    "Ao fim de um \".and\", \".or\" ou \".not\" espera-se \".\"", "E esperado um \" \' \" depois de um caracter valido",
    "Era esperado um n depois do \"a\"", "Era esperado um a, o, ou n depois do ponto", "Era esperado \"r\" depois do \"o\"",
    "Era esperado \"o\" depois do \"n\"", "Era esperado \"d\" depois do \"n\"", "Era esperado \"t\" depois do \"o\"",
    "Caracter invalido"};

void error(int e){
    printf("\nErro Sintatico %i na linha %i e na coluna %i: %s\n", e, get_line(), get_column(), ErrosSintaticos[e]);
    exit(0);
}

void error2(int e){
    printf("\nErro Semantico %i na linha %i e na coluna %i: %s\n", e, get_line(), get_column(), ErrosSemanticos[e]);
    exit(0);
}

void execution_erro(int e){
    printf("\nErro de Execucao: %s\n", e, get_line(), get_column(), ErrosSistema[e]);
    exit(0);
}

void erro_lexico(int e){
    printf("\nErro Lexico %i na linha %i e na coluna %i: %s\n", e, get_line(), get_column(), ErrosLexicos[e]);
    exit(0);
}