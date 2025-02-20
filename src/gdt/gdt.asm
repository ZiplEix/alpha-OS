section .asm
global gdt_load

gdt_load:
    mov     eax, [esp + 4]                  ; Load the address of the GDT
    mov     [gdt_deescriptor + 2], eax      ; Store the address of the GDT in the GDT descriptor
    mov     ax, [esp + 8]
    mov     [gdt_deescriptor], ax           ; Store the size of the GDT in the GDT descriptor
    lgdt    [gdt_deescriptor]               ; Load the GDT
    ret

section .data
gdt_deescriptor:
    dw      0                               ; Size of the GDT
    dd      0                               ; Start address of the GDT