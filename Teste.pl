pl Teste

var 
    int a, b, i
    char c, d
    real x, y, result
endvar

fwd int s(int, int)

fwd proc print(int, int, int)

fwd proc semparametro(noparam)

fwd real pow(real, real)

int s(int a, int b)
    int x, c
    return (a+b)
endfunc

proc print(int a, int b, int c)
    real x
    int h
    h = s(a, b) + s(b, c)
    display a dup 5, b, c
    return
endproc

proc semparametro()
    display a, b, i
endproc

real pi()
    return (3.14)
endfunc

real pow(real x, real y)
    int i
    real result
    result = x
    for(i = 1, i<y, i=i+1)
        result = result * x
    endfor
    return (result)
endfunc

prog
    x = pi()
    call semparametro()
    c = a
    d = a + b  
    keyboard a, b
    if(a.and.c)
        a = c
    endif
    if((a*c+2+8>b*d+5*a*b).or.(b+1>c*5).and..not.(c>a))
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
    x = 4.0
    y = 8.0
    result = pow(x, y)
    display result
    return (s(a, b))
endprog