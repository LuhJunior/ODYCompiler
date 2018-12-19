# ODYCompiler 
Compilador para a linguaguem PL*

# Compilação

Para compilar é necessário possuir o gcc instalado.
- Você irá precisar dos arquivos: odc.c, lexico.c, sintatico.c, tabela_simbolos.c, vector.c e error.c.
Bem como os seus repectivos headers
- Utilize o comando gcc odc.c lexico.c sintatico.c tabela_simbolos.c vector.c error.c -o 
nome_do_executavel.exe
- Caso esteja no linux nome_do_executavel não possui o .exe no final.

# Como Usar

Para utilizar o analisador léxico você terá que passar na linha de comando o nome_do_executal e o 
arquivo com o texto ao qual quer analisar.

Exemplo: C>analisador entrada.txt

# Sobre

O programa tem como objetivo fazer uma análise léxica, sintática e semântica de um código escrito na
linguagem PL respectivamente. No final ele mostrará todos os tokens pegos ao longo da análise e a
confirmação se está escrito corretamente ou não. Durante a análise caso o programa encontre algum erro
ele encerrará emediatamente e dirá onde está o erro.
