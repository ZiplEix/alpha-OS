;***************************************************
; Bootloader - stage 2
;    - Load the kernel from the disk
;
;      - By Baptiste Leroyer
;***************************************************

org 0x0                                             ; offset to 0, set the segment later

bits 16                                             ; still in real mode

; Loaded at linear adress 0x100000

jmp main

;***************************************************
;    Print a string
;    SI = pointer to string
;    DS=>SI: 0 terminated string
;***************************************************

Print:
    lodsb                   ; load next byte from si to al
    or al, al               ; Check if the character is null
    jz PrintDone
    mov ah, 0eh             ; get next character
    int 10h                 ; call BIOS video interrupt
    jmp Print

PrintDone:
    ret

;***************************************************
;    Second stage loader entry point
;***************************************************

main:
    cli                     ; disable interrupts
    push    cs              ; ensure DS=CS
    pop     ds

    mov     si, Msg
    call    Print

    cli
    hlt

;***************************************************
;    Data section
;***************************************************

Msg	    db  "Preparing to load operating system...", 13, 10,0
