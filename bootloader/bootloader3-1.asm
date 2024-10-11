;***************************************************
; bootloader for Intel/AMD x86 CPUs
;
; This bootloader loads the second sector from the floppy
; and then jumps to execute it
;
; Sector 1
;
;      - By Baptiste Leroyer
;***************************************************

bits    16                          ; We are still in 16 bit Real Mode

org     0x7C00                      ; We are loaded by BIOS at 0x7C00

start:          jmp loader          ; jump over OEM block

;*************************************************
; OEM Parameter block / BIOS Parameter Block
;*************************************************

TIMES 0Bh-$+start DB 0

bpbBytesPerSector:   DW 512
bpbSectorsPerCluster: DB 1
bpbReservedSectors:  DW 1
bpbNumberOfFATs:     DB 2
bpbRootEntries:      DW 224
bpbTotalSectors:     DW 2880
bpbMedia:           DB 0xF0
bpbSectorsPerFAT:   DW 9
bpbSectorsPerTrack: DW 18
bpbHeadsPerCylinder: DW 2
bpbHiddenSectors:   DD 0
bpbTotalSectorsBig: DD 0
bsDriveNumber:      DB 0
bsUnused:          DB 0
bsExtBootSignature: DB 0x29
bsSerialNumber:     DD 0xa0a1a2a3
bsVolumeLabel:      DB "MOS FLOPPY "
bsFileSystem:       DB "FAT12   "

;***************************************************
; Print a string
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

;*************************************************
; Bootloader Entry Point
;*************************************************

loader:

.Reset:
    ; Reset floppy drive
    mov ah, 0               ; reset floppy disk function
    mov dl, 0               ; drive 0 is floppy drive
    int 0x13                ; call BIOS
    jc .Reset                ; If Carry Flag (CF) is set, there was an error. Try resetting again

    ; Read second sector from floppy
    mov ax, 0x1000          ; load address where to read the sector
    mov es, ax              ; es points to segment 0x1000
    xor bx, bx              ; bx = 0

    mov ah, 0x02            ; read floppy sector function
    mov al, 1               ; read 1 sector
    mov ch, 1               ; we are reading the second sector past us
    mov cl, 2               ; sector to read (the second sector)
    mov dh, 0               ; head number
    mov dl, 0               ; drive number (floppy drive)
    int 0x13                ; call BIOS - Read the sector

    jmp 0x1000:0x0          ; jump to execute the sector!

; Fill the rest of the sector with zeros
times 510 - ($-$$) db 0

dw 0xAA55                   ; Boot Signature

; End of sector 1, beginning of sector 2 ---------------------------------
