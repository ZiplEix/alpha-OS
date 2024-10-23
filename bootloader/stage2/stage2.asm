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

%include "stdio.inc"                   ; basic i/o routines
%include "gdt.inc"                     ; GDT routines
%include "A20.inc"                     ; A20 activation routines
%include "Fat12.inc"                   ; FAT12 routines
%include "common.inc"                  ; common definition

;***************************************************
;       Data Section
;***************************************************

LoadingMsg  db  "Searching for Operating System...", 0x0D, 0x0A, 0x00
msgFailure  db 0x0D, 0x0A, "*** FATAL: MISSING OR CURRUPT KRNLDR.SYS. Press Any Key to Reboot", 0x0D, 0x0A, 0x0A, 0x00
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
    mov     ax, 0x0                             ; stack begin at 0x9000-0xffff
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
    ;   Initialize filesysteme
    ;-------------------------------------------
    call    LoadRoot                            ; Load root directory table

    ;-------------------------------------------
    ;   Load Kernel
    ;-------------------------------------------
    mov     ebx, 0                              ; BX:BP points to buffer to load to
    mov     bp, IMAGE_RMODE_BASE
    mov     si, ImageName
    call    LoadFile                            ; Load the file
    mov     dword [ImageSize], ecx              ; Store the size of the file
    cmp     ax, 0                               ; check for success
    je      EnterStage3                         ; Yep jump to stage 3
    mov     si, msgFailure
    call    Puts16
    mov     ah, 0
    int     0x16                                ; Wait for a key
    int     0x19                                ; Reboot
    cli
    hlt

    ;-------------------------------------------
    ;   Go into pmode
    ;-------------------------------------------
EnterStage3:
    cli                                         ; Disable and clear interrupts
    mov     eax, cr0                            ; set bit 0 in cr0 --enter pmode
    or      eax, 1
    mov     cr0, eax

    jmp     CODE_DESC:Stage3                    ; far jump to fix CS. Code selector is 0x8

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
    mov     ax, DATA_DESC                       ; Set data segments to data selector (0x10)
    mov     ds, ax
    mov     ss, ax
    mov     es, ax
    mov     esp, 90000h                         ; Set stack pointer to 0x90000

    ;-------------------------------------------
    ;       Clear screen and print success
    ;-------------------------------------------
    ; call    ClrScr32
    ; mov     ebx, msg
    ; call    Puts32

    ;-------------------------------------------
    ;       Copy kernel to 1MB
    ;-------------------------------------------
    CopyImage:
        mov     eax, dword [ImageSize]
        movzx   ebx, word [bpbBytesPerSector]
        mul     ebx
        mov     ebx, 4
        div     ebx
        cld
        mov     esi, IMAGE_RMODE_BASE
        mov     edi, IMAGE_PMODE_BASE
        mov     ecx, eax
        rep     movsd                           ; Copy image to its protected mode address

        call    ClrScr32
        mov     ebx, DebugMsg1
        call    Puts32

    ;-------------------------------------------
    ;       Jump to kernel
    ;-------------------------------------------
    mov     eax, CODE_DESC
    mov     ebx, IMAGE_PMODE_BASE

    jmp     CODE_DESC:IMAGE_PMODE_BASE

;***************************************************
;       Stop execution
;***************************************************

STOP:
    cli
    hlt

msg db  0x0A, 0x0A, 0x0A,   "               <[ AlphaOS Bootloader ]>"
    db  0x0A, 0x0A,         "                    Hello World !", 0
