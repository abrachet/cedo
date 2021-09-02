    .data
    .type a,@object
    .size a, 4
    .global a
    .align 1
a:
    .long 1

    .type b,@object
    .size b, 8
    .global b
    .align 1
b:
    .quad a

    .type c,@object
    .size c, 8
    .global c
    .align 1
c:
    .quad c

    .ident "cedo"
