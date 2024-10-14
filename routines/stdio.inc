;**********************************************************
;       stdio.inc
;               - Input/Output routines
;
;       AlphaOS
;**********************************************************

%ifndef __STDIO_INC__
%define __STDIO_INC__

;**********************************************************
;       Puts16()
;               - Print a null-terminated string
;       DS=>SI: 0 terminated string
;**********************************************************

bits    16

Puts16:
    pusha
    .LoopPuts16:
        lodsb
        or      al, al
        jz      .EndPuts16
        mov     ah, 0eh
        int     10h
        jmp     .LoopPuts16
    .EndPuts16:
        popa
        ret

%endif  ; __STDIO_INC__
