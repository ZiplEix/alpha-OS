[BITS 32]

global _start
extern c_start
extern __exit

section .asm

_start:
    call    c_start
    call    __exit
    ret
