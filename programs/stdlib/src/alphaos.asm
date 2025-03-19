[BITS 32]

; void print(const char *filename)
global print:function
print:
    push    ebp
    mov     ebp, esp
    push    dword[ebp + 8]
    mov     eax, 1 ; kernel print
    int     0x80
    add     esp, 4
    pop     ebp
    ret
