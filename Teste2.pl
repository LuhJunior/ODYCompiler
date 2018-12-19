pl Teste2 
var
    real a, b, c
endvar
real pow(real x, real y)
    real i
    real ax
    ax = x
    for(i = 1.0, i<y, i = i+1.0)
        ax = ax * x
    endfor
    return(ax)
endfunc
prog
    keyboard a, b
    c = pow(a, b)
    display c
endprog