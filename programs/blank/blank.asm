[BITS 32]

section .asm

global _start

_start:
    call    getkey
    push    message
    mov     eax, 1 ; command print
    int     0x80
    add     esp, 4

    jmp $

; wait for a key to be pressed, return the key in eax
getkey:
    mov     eax, 2 ; command getkey
    int     0x80
    cmp     eax, 0x00
    je      getkey
    ret

section .data
message: db 'Talking with the kernel !', 0
