[BITS 32]

section .asm

global _start

_start:
    push     20
    push     30

    mov     eax, 0 ; Command 0 SUM
    int     0x80

    add     esp, 8 ; Clean the stack, 8 because we pushed 2 dwords, adding because the stack grows down

    jmp $
