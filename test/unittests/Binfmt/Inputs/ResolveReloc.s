
.section .test32
    .long string0
    .long string0
    .long string4

.section .test64
    .quad string0

.section .test_string0
string0:
    .asciz "String 0"

.section .test_string4
    .asciz "bad"
string4:
    .asciz "String 4"
