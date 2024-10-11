;***************************************************
; bootloader for Intel/AMD x86 CPUs
;
; This bootloader prints a message to the screen
; and then halts the system
;
;      - By Baptiste Leroyer
;***************************************************

org 0x7C00                  ; BIOS loads boot sector at 0x7C00 (ensure all intruction are relative to this address)

bits 16                     ; 16 bit real mode

Start:
    jmp loader

;***************************************************
;	OEM Parameter block
;***************************************************

bpbOEM: 	            DB "Alpha OS" ; The name of the OS

bpbBytesPerSector:  	DW 512
bpbSectorsPerCluster: 	DB 1
bpbReservedSectors: 	DW 1
bpbNumberOfFATs: 	    DB 2
bpbRootEntries: 	    DW 224
bpbTotalSectors: 	    DW 2880
bpbMedia: 	            DB 0xF0
bpbSectorsPerFAT: 	    DW 9
bpbSectorsPerTrack: 	DW 18
bpbHeadsPerCylinder: 	DW 2
bpbHiddenSectors: 	    DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber: 	        DB 0
bsUnused: 	            DB 0
bsExtBootSignature: 	DB 0x29
bsSerialNumber:	        DD 0xa0a1a2a3
bsVolumeLabel: 	        DB "MOS FLOPPY "
bsFileSystem: 	        DB "FAT12"

msg db "Hello, World !", 0

;***************************************************
;    Print a string
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
;	Bootloader Entry Point
;***************************************************

loader:
    xor ax, ax
    mov ds, ax
    mov es, ax

    mov si, msg             ; load message to si
    call Print

    cli                     ; disable and clear all interrupts
    hlt                     ; halt the system

times 510 - ($-$$) db 0     ; fill the rest of the sector with zeros

dw 0xAA55                   ; Boot signature
