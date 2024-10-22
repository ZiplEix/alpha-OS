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

;**********************************************************
;       32 bit Protected Mode Routines
;**********************************************************

bits 32

%define VIDEO_MEMORY    0xB8000                 ; Video memory address
%define COLS            80                      ; Number of columns of the VGA screen
%define ROWS            25                      ; Number of rows of the VGA screen
%define CHAR_ATTRIBUTE  63                      ; character attribute (White text on light blue background) (like windows 2 xD)

_CurX db 0                                      ; Current X position
_CurY db 0                                      ; Current Y position

;**********************************************************
;       Putch32()
;               - Print a character to screen
;       BL => Character to print
;**********************************************************

Putch32:
    pusha
    mov     edi, VIDEO_MEMORY                   ; Set video memory address

    ;--------------------------------------------------
    ;   Get current position
    ;--------------------------------------------------
    xor     eax, eax                            ; Clear EAX

    ;--------------------------------------------------
    ;   currentPos = x + y * COLS. x ans y are in _CurX and _CurY
    ;   Because there are two bytes per character, COLS = number of characters in a line.
    ;   We have to multiply this by 2 to get the number of bytes per line. This is the screen width,
    ;   so multiply screen with * _CurY to get the number of bytes to skip to get to the current line.
    ;------------------------------------------------
    mov     ecx, COLS * 2                       ; mode 7 has 2 bytes per char, so it's COLS * 2 bytes per line
    mov     al, byte [_CurY]                    ; Get current Y position
    mul     ecx                                 ; EAX = EAX * ECX => EAX = _CurY * COLS * 2
    push    eax                                 ; Save EAX

    ;--------------------------------------------------
    ;    Now y * screen width is in eax. Just have to add _CurX. But, _CurX is in characters, not bytes.
    ;   Because there are 2 bytes per character, we have to multiply _CurX by 2 to get the number of bytes to skip.
    ;--------------------------------------------------
    mov     al, byte [_CurX]                    ; Get current X position
    mov     cl, 2
    mul     cl                                  ; EAX = EAX * 2 => EAX = _CurX * 2
    pop     ecx                                 ; Get _CurY * COLS * 2
    add     eax, ecx                            ; EAX = EAX + ECX => EAX = _CurY * COLS * 2 + _CurX * 2

    ;--------------------------------------------------
    ;   Now eax contains the offset adress to draw the character at.
    ;   So we just have to add it to the base video memory address (stored in EDI)
    ;--------------------------------------------------
    xor     ecx, ecx                            ; Clear ECX
    add     edi, eax                            ; Add offset to video memory address

    ;--------------------------------------------------
    ;   Watch for new line
    ;--------------------------------------------------
    cmp     bl, 0x0A                            ; Is it a new line?
    je      .Row                                ; Yes, go to next row

    ;--------------------------------------------------
    ;   Print character
    ;--------------------------------------------------
    mov     dl, bl                              ; Get character to print
    mov     dh, CHAR_ATTRIBUTE                  ; Get character attribute
    mov     word [edi], dx                      ; Write to video display

    ;--------------------------------------------------
    ;   Update next position
    ;--------------------------------------------------
    inc     byte [_CurX]                        ; Increment X position
    jmp    .done

    ;--------------------------------------------------
    ;   Go to next row
    ;--------------------------------------------------
    .Row:
        mov     byte [_CurX], 0                  ; Reset X position
        inc     byte [_CurY]                     ; Increment Y position

    ;--------------------------------------------------
    ;   Restore regoster & return
    ;--------------------------------------------------
    .done:
        popa
        ret

;**********************************************************
;       Puts32()
;               - Print a null-terminated string
;       EBX => address of the null terminated string to print
;**********************************************************

Puts32:
    ;--------------------------------------------------
    ;   Store registers
    ;--------------------------------------------------
    pusha
    push    ebx                                 ; Copy the string address to EBX
    pop     edi

    .loop:
        ;------------------------------------------
        ;   Get character
        ;------------------------------------------
        mov     bl, byte [edi]                  ; Get next character
        cmp     bl, 0                           ; Is it null?
        je      .done                           ; Yes, end of string

        ;------------------------------------------
        ;   Print character
        ;------------------------------------------
        call    Putch32

        ;------------------------------------------
        ;   Next character
        ;------------------------------------------
        inc     edi                             ; Next character
        jmp     .loop

    .done:
        ;------------------------------------------
        ;   Update hardware cursor
        ;------------------------------------------
        ; it's more efficient to update the cursor after displaying the complete string because direct VGA is slow

        mov     bh, byte [_CurY]                ; Get current Y position
        mov     bl, byte [_CurX]                ; Get current X position
        call    MovCur                          ; Update cursor position

        popa
        ret

;**********************************************************
;       MovCur()
;               - Update hardware cursor
;       BH => Y position
;       BL => X position
;**********************************************************

bits 32

MovCur:
    pusha
    ;--------------------------------------------------
    ;   Get current position
    ;--------------------------------------------------
    ; Here, _CurX and _CurY are relitave to the current position on screen, not in memory.
	; That is, we don't need to worry about the byte alignment we do when displaying characters,
	; so just follow the forumla: location = _CurX + _CurY * COLS

    xor     eax, eax
    mov     ecx, COLS
    mov     al, bh                              ; Get Y position
    mul     ecx                                 ; EAX = EAX * ECX => EAX = _CurY * COLS
    add     al, bl                              ; EAX = EAX + BL => EAX = _CurY * COLS + _CurX
    mov     ebx, eax

    ;--------------------------------------------------
    ;   Set low byte index to VGA register
    ;--------------------------------------------------
    mov     al, 0x0f
    mov     dx, 0x03D4
    out     dx, al

    mov     al, bl
    mov     dx, 0x03D5
    out     dx, al                              ; low byte

    ;--------------------------------------------------
    ;   Set high byte index to VGA register
    ;--------------------------------------------------
    xor     eax, eax

    mov     al, 0x0e
    mov     dx, 0x03D4
    out     dx, al

    mov     al, bh
    mov     dx, 0x03D5
    out     dx, al                              ; high byte

    popa
    ret

;**********************************************************
;       ClearScreen32()
;               - Clear the screen
;**********************************************************

bits 32

ClrScr32:
    pusha
    cld
    mov     edi, VIDEO_MEMORY
    mov     cx, 2000                            ; 80 * 25 * 2
    mov     ah, CHAR_ATTRIBUTE
    mov     al, ' '
    rep     stosw

    mov     byte [_CurX], 0
    mov     byte [_CurY], 0
    popa
    ret

;**********************************************************
;       GotoXY()
;               - Set current X/Y location
;       AL => Y position
;       AH => X position
;**********************************************************

bits 32

GotoXY:
    pusha
    mov     byte [_CurX], ah
    mov     byte [_CurY], al
    popa
    ret

%endif  ; __STDIO_INC__
