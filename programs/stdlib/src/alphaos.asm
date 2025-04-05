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

; int __getkey()
global __getkey:function
__getkey:
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

; void __process_load_start(const char *filename)
global __process_load_start:function
__process_load_start:
    push    ebp
    mov     ebp, esp
    push    dword[ebp + 8] ; filename
    mov     eax, 6 ; kernel process_load_start
    int     0x80
    add     esp, 4
    pop     ebp
    ret

; int __system(struct command_arguments* args)
global __system:function
__system:
    push    ebp
    mov     ebp, esp
    push    dword[ebp + 8] ; args
    mov     eax, 7 ; process_systrem (run a system command based on the arguments)
    int     0x80
    add     esp, 4
    pop     ebp
    ret

; void __process_get_arguments(struct process_aguments *arguments)
global __process_get_arguments:function
__process_get_arguments:
    push    ebp
    mov     ebp, esp
    push    dword[ebp + 8] ; arguments
    mov     eax, 8 ; kernel process_get_arguments
    int     0x80
    add     esp, 4
    pop     ebp
    ret
