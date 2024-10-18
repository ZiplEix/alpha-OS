;***************************************************
; Bootloader - stage 2
;    - Load the kernel from the disk
;
;      - By Baptiste Leroyer
;***************************************************

bits    16

; 0x500 through 0x7DFF is unused above the BIOS data area
; current instruction is loaded at 0x500

org     0x500

jmp     main                                    ; Go to start

;***************************************************
;       Preprocessor directives
;***************************************************

%include "routines/stdio.inc"                   ; basic i/o routines
%include "routines/gdt.inc"                     ; GDT routines
%include "routines/A20.inc"                     ; A20 activation routines

;***************************************************
;       Data Section
;***************************************************

LoadingMsg  db  "Searching for Operating System...", 0x0D, 0x0A, 0x00
DebugMsg1   db  "Debug A", 0x0D, 0x0A, 0x00
DebugMsg2   db  "Debug B", 0x0D, 0x0A, 0x00

;***************************************************
;       STAGE 2 ENTRY POINT
;
;           - Store BIOS infos
;           - Load Kernel
;           - Install GDT; go into PMode
;           - Jump to Stage 3
;***************************************************

main:
    ;-------------------------------------------
    ;   Setup Segments and stack
    ;-------------------------------------------
    cli                                         ; Disable and clear interrupts
    xor     ax, ax                              ; null segments
    mov     ds, ax
    mov     es, ax
    mov     ax, 0x9000                          ; stack begin at 0x9000-0xffff
    mov     ss, ax
    mov     sp, 0xFFFF
    sti                                         ; Enable interrupts

    ;-------------------------------------------
    ;   Install GDT
    ;-------------------------------------------
    call    InstallGDT

    ;-------------------------------------------
    ;   Enable A20
    ;-------------------------------------------
    call    EnableA20_KKbrd

    ;-------------------------------------------
    ;   Display loading message
    ;-------------------------------------------
    mov     si, LoadingMsg
    call    Puts16

    ;-------------------------------------------
    ;   Go into pmode
    ;-------------------------------------------
EnterStage3:
    cli                                         ; Disable and clear interrupts
    mov     eax, cr0                            ; set bit 0 in cr0 --enter pmode
    or      eax, 1
    mov     cr0, eax

    jmp     CODE_DESC:Stage3                          ; far jump to fix CS. Code selector is 0x8

    ; DO NOT BY ANY MEANS RE-ANABLE INTERRUPTS ! DOING SO WILL TRIPLE FAULT THE CPU !!!!!!
    ; will be fix in Stage 3 (hopefully...)

;***************************************************
;      ENTRY POINT FOR STAGE 3
;***************************************************

bits    32                                      ; Finally in 32 bits mode !!

Stage3:
    ;-------------------------------------------
    ;       Set registers
    ;-------------------------------------------
    mov     ax, DATA_DESC                            ; Set data segments to data selector (0x10)
    mov     ds, ax
    mov     ss, ax
    mov     es, ax
    mov     esp, 90000h                         ; Set stack pointer to 0x90000

    ;-------------------------------------------
    ;       Clear screen and print success
    ;-------------------------------------------
    call    ClrScr32
    mov     ebx, msg
    call    Puts32

;***************************************************
;       Stop execution
;***************************************************

STOP:
    cli
    hlt

msg db  0x0A, 0x0A, 0x0A, "               <[ AlphaOS Bootloader ]>"
    db  0x0A, 0x0A,             "           Hello World !", 0
    