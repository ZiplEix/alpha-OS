ORG     0
BITS    16

jmp     0x07C0:start                        ; Jump to the start of the boot loader

start:
    cli                                     ; Disable interrupts
    mov     ax, 0x07C0
    mov     ds, ax
    mov     es, ax
    mov     ax, 0x00
    mov     ss, ax
    mov     sp, 0x7C00
    sti                                     ; Enable interrupts

    mov     si, message
    call    print
    jmp     $

print:
    mov     bx, 0

    .loop:
        lodsb
        cmp     al, 0
        je      .done
        call    print_char
        jmp     .loop

    .done:
        ret

print_char:
    mov     ah, 0eh
    int     0x10
    ret

message: db "Hello, World !", 0

times 510-($-$$) db 0
dw      0xAA55
