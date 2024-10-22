;**********************************************************
;       A20.inc
;               - Anable A20 adress line
;               - NOT WRITTEN BY ME !
;
;       AlphaOS
;**********************************************************

%ifndef __A20_INC__
%define __A20_INC__

bits    16

;----------------------------------------------------------
;       Enable A20 line through keyboard controller (8042) (most common and portable way)
;----------------------------------------------------------

EnableA20_KKbrd:
    cli
    push    ax
    mov     al, 0xdd                            ; send anable a20 adress line command to controller
    out     0x64, al
    pop     ax
    ret

;--------------------------------------------
; Enables a20 line through output port
;--------------------------------------------

EnableA20_KKbrd_Out:
	cli
	pusha

    call    wait_input
    mov     al,0xAD
    out     0x64,al		                        ; disable keyboard
    call    wait_input

    mov     al,0xD0
    out     0x64,al		                        ; tell controller to read output port
    call    wait_output

    in      al,0x60
    push    eax		                            ; get output port data and store it
    call    wait_input

    mov     al,0xD1
    out     0x64,al		                        ; tell controller to write output port
    call    wait_input

    pop     eax
    or      al,2		                        ; set bit 1 (enable a20)
    out     0x60,al		                        ; write out data back to the output port

    call    wait_input
    mov     al,0xAE		                        ; enable keyboard
    out     0x64,al

    call    wait_input
	popa
    sti
    ret

	; wait for input buffer to be clear

wait_input:
    in      al,0x64
    test    al,2
    jnz     wait_input
    ret

	; wait for output buffer to be clear

wait_output:
    in      al,0x64
    test    al,1
    jz      wait_output
    ret

;--------------------------------------
; Enables a20 line through bios
;--------------------------------------

EnableA20_Bios:
	pusha
	mov	ax, 0x2401
	int	0x15
	popa
	ret

;-------------------------------------------------
; Enables a20 line through system control port A
;-------------------------------------------------

EnableA20_SysControlA:
	push	ax
	mov	al, 2
	out	0x92, al
	pop	ax
	ret

%endif  ; __A20_INC__
