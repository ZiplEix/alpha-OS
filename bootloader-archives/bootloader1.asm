;***************************************************
; Most simple bootloader for Intel/AMD x86 CPUs
;
; This bootloader does nothing but halt the system
;
; To compile and run this bootloader:
;   nasm -f bin bootloader1.asm -o bootloader1.bin
;
;      - By Baptiste Leroyer
;***************************************************

org 0x7C00                  ; BIOS loads boot sector at 0x7C00 (ensure all intruction are relative to this address)

bits 16                     ; 16 bit real mode

Start:
    cli                     ; disable and clear all interrupts
    hlt                     ; halt the system

times 510 - ($-$$) db 0    ; fill the rest of the sector with zeros

dw 0xAA55                   ; Boot signature
