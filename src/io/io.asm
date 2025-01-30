section .asm

global insb
global insw
global outb
global outw

insb:
    push    ebp
    mov     ebp, esp

    xor     eax, eax
    mov     edx, [ebp + 8]     ; port
    in      al, dx

    pop     ebp
    ret

insw:
    push    ebp
    mov     ebp, esp

    xor     eax, eax
    mov     edx, [ebp + 8]     ; port
    in      ax, dx

    pop     ebp
    ret

outb:
    push    ebp
    mov     ebp, esp

    mov     eax, [ebp + 12]    ; data
    mov     edx, [ebp + 8]     ; port
    out     dx, al

    pop     ebp
    ret

outw:
    push    ebp
    mov     ebp, esp

    mov     eax, [ebp + 12]    ; data
    mov     edx, [ebp + 8]     ; port
    out     dx, ax

    pop     ebp
    ret
