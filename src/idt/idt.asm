section .asm

extern int21h_handler
extern no_interrupt_handler
extern isr80h_handler
extern interrupt_handler

global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper
global interrupt_pointer_table

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

idt_load:
    push    ebp
    mov     ebp, esp

    mov     ebx, [ebp + 8]  ; idt
    lidt    [ebx]

    pop     ebp
    ret

no_interrupt:
    pushad

    call    no_interrupt_handler

    popad
    iret

%macro interrupt 1
    global int%1
    int%1:
        ; Interupts frame start
        ; already pushed by the processor upon entry to this interrupt
        ; uint32_t ip
        ; uint32_t cs
        ; uint32_t flags
        ; uint32_t sp
        ; uint32_t ss

        ; push the general purpose registers to the stack
        pushad

        ; interrupt frame end

        ; push the stack pointer so that we are pointing to the interrupt frame
        push    esp
        push    dword %1

        call    interrupt_handler

        add     esp, 8
        popad
        iret
%endmacro

%assign i 0
%rep 512
    interrupt i
    %assign i i+1
%endrep

isr80h_wrapper:
    ; Interupts frame start
    ; already pushed by the processor upon entry to this interrupt
    ; uint32_t ip
    ; uint32_t cs
    ; uint32_t flags
    ; uint32_t sp
    ; uint32_t ss

    ; push the general purpose registers to the stack
    pushad

    ; interrupt frame end

    ; push the stack pointer so that we are pointing to the interrupt frame
    push    esp

    ; EAX holds the commands. push it to the satck for isr80h_handler
    push    eax
    call    isr80h_handler
    mov     dword[tmp_res], eax
    add     esp, 8 ; pop the stack pointer and the command, add 8 to the satck pointer so similar to poping 2 32 bit values

    ; restore general purpose register for user land
    popad
    mov     eax, dword[tmp_res]
    iretd

section .data

; stores the return result from the isr80h_handler
tmp_res: dd 0

%macro interrupt_array_entry 1
    dd      int%1
%endmacro

interrupt_pointer_table:
%assign i 0
%rep 512
    interrupt_array_entry i
    %assign i i+1
%endrep
