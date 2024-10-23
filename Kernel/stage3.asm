;***************************************************
; Bootloader - stage 2
;    - A basic 32 bit binary kernel running
;
;      - By Baptiste Leroyer
;***************************************************

org	0x100000			                        ; Kernel starts at 1 MB

bits	32				                        ; 32 bit code

jmp	Stage3				                        ; jump to entry point

%include "stdio.inc"

msg db  0x0A, 0x0A, 0x0A,   "               <[ AlphaOS Kernel ]>"
    db  0x0A, 0x0A,         "           MOS 32 Bit Kernel Executing", 0x0A, 0

Stage3:
    mov	    ax, 0x10		                    ; set data segments to data selector (0x10)
	mov	    ds, ax
	mov	    ss, ax
	mov	    es, ax
	mov	    esp, 90000h		                    ; stack begins from 90000h

    ;-------------------------------------------
    ;   Clear screen and print success
    ;-------------------------------------------
    call	ClrScr32
    mov     ebx, msg
    call	Puts32

    ; .infiniteloop:
    ;     jmp .infiniteloop

    ;-------------------------------------------
    ;   Stop execution
    ;-------------------------------------------
    cli
    hlt
