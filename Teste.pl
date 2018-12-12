pl Teste

var 
    int a, b
    char c, d
    real x, y
endvar
fwd int s(int, int)
fwd proc print(int, int, int)
int s(int a, int b)
    int x, c
    return (a+x)
endfunc

proc print(int a, int b, real c)
    int h
    real x
    h = a
    display a, b, c
endproc

prog 
c = a
d = a + b
keyboard a, b
call print(a, b, x)
return (s(a, b))
endprog
