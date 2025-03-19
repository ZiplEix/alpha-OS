[BITS 32]

section .asm

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

; void __putchar(char c)
global __putchar:function
__putchar:
    push    ebp
    mov     ebp, esp
    push    dword[ebp + 8] ; c
    mov     eax, 3 ; kernel putchar
    int     0x80
    add     esp, 4
    pop     ebp
    ret

; void *__malloc(size_t size)
global __malloc:function
__malloc:
    push    ebp
    mov     ebp, esp
    push    dword[ebp + 8] ; size
    mov     eax, 4 ; kernel malloc
    int     0x80
    add     esp, 4
    pop     ebp
    ret

; void __free(void *ptr)
global __free:function
__free:
    push    ebp
    mov     ebp, esp
    push    dword[ebp + 8] ; ptr
    mov     eax, 5 ; kernel free
    int     0x80
    add     esp, 4
    pop     ebp
    ret
