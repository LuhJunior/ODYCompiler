pl Teste
var 
    int a, b, i
    char c, d
    real x, y
endvar
fwd int s(int, int)
fwd proc print(int, int, int)
fwd proc semparametro(noparam)
int s(int a, int b)
    int x, c
    return (a+b)
endfunc

proc print(int a, int b, int c)
    int h
    real x
    h = s(a, b) + s(b, c)
    display a dup 5, b, c
    return
endproc
proc semparametro()
    display a, b , i
endproc

prog 
c = a
d = a + b
keyboard a, b
if(a.and.c)
    a = c
endif
if(a*c+2+8>b*d+5*a*b.or.(b+1>c*5).and.c.or..not.(c>a))
    call print(a, b, a)
else
    a = 0
endif
i = 0
while(i<40)
    a = i*a
endwhile
for(i=1, i<40, i=i+1)
    i = i*i
endfor
return (s(a, b))
endprog
