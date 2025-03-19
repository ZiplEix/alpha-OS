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

; int getkey()
global getkey:function
getkey:
    push    ebp
    mov     ebp, esp
    mov     eax, 2 ; kernel getkey
    int     0x80

    pop     ebp
    ret

; void *__malloc(size_t size)
global __malloc:function
__malloc:
    push    ebp
    mov     ebp, esp
    push    dword[ebp + 8]
    mov     eax, 4 ; kernel malloc
    int     0x80
    add     esp, 4
    pop     ebp
    ret
