ORG     0x7c00
BITS    16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; BIOS parameter block
jmp     short start
nop

; FAT16 Header
OEMIdentifier       db "ALPHAOS "
BytesPerSector      dw 0x200
SectorsPerCluster   db 0x80
ReservedSectors     dw 200
FATCopies           db 0x02
RootDirEntries      dw 0x40
NumSectors          dw 0x0
MediaType           db 0xF8
SectorsPerFat       dw 0x100
SectorsPerTrack     dw 0x20
NumberOfHeads       dw 0x40
HiddenSctors        dd 0x00
SectorBig           dd 0x773594

; Extended BPB (Dos 4.0)
DriveNumber         db 0x80
WinNTBit            db 0x00
Signature           db 0x29
VolumeID            dd 0xD105
VolumeIDString      db "ALPHAOS BOO"
SystemIDString      db "FAT16   "

start:
    jmp     0:step2                         ; Jump to the start of the boot loader

step2:
    cli                                     ; Disable interrupts
    mov     ax, 0x00
    mov     ds, ax
    mov     es, ax
    mov     ss, ax
    mov     sp, 0x7C00
    sti                                     ; Enable interrupts

.load_protected:
    cli
    lgdt[gdt_descriptor]
    mov     eax, cr0
    or      eax, 0x1
    mov     cr0, eax
    jmp     CODE_SEG:load32

    jmp $

; GDT
gdt_start:
gdt_null:
    dd      0x0
    dd      0x0

; offset 0x8
gdt_code:                                   ; CS should point to this, default value
    dw      0xFFFF                          ; segment limit first 0-15 bits
    dw      0x0                             ; base first 0-15 bits
    db      0x0                             ; base 16-23 bits
    db      0x9a                            ; access byte
    db      11001111b                       ; high 4 bit flag and the low 4 bit flags
    db      0x0                             ; base 24-31 bits

; offset 0x10
gdt_data:                                   ; DS, SS, ES, FS, GS should point to this, default value
    dw      0xFFFF                          ; segment limit first 0-15 bits
    dw      0x0                             ; base first 0-15 bits
    db      0x0                             ; base 16-23 bits
    db      0x92                            ; access byte
    db      11001111b                       ; high 4 bit flag and the low 4 bit flags
    db      0x0                             ; base 24-31 bits

gdt_end:

gdt_descriptor:
    dw      gdt_end - gdt_start - 1
    dd      gdt_start

[BITS 32]
load32:
    mov     eax, 1
    mov     ecx, 100
    mov     edi, 0x0100000
    call    ata_lba_read
    jmp     CODE_SEG:0x0100000

ata_lba_read:
    mov     ebx, eax                        ; save the LBA
    ; send the highest 8 bits og the lba to hard fisk controller
    shr     eax, 24
    or      eax, 0xE0                       ; select the master drive
    mov     dx, 0x1F6
    out     dx, al
    ; finished sending the highest 8 bits of the lba
    ; send the total sector to read
    mov     eax, ecx
    mov     dx, 0x1F2
    out     dx, al
    ; finished sending the total sector to read

    ; send more bits of the LBA
    mov     eax, ebx                        ; restore the saved lba
    mov     dx, 0x1F3
    out     dx, al
    ; finished sending more bits of the LBA

    ; send more bits of the LBA
    mov     dx, 0x1F4
    mov     eax, ebx                        ; restore the backup LBA
    shr     eax, 8
    out     dx, al
    ; finished sending more bits of the LBA

    ; send upper 16 bits of the LBA
    mov     dx, 0x1F5
    mov     eax, ebx                        ; restore the backup LBA
    shr     eax, 16
    out     dx, al
    ; finished sending upper 16 bits of the LBA

    mov     dx, 0x1F7
    mov     al, 0x20
    out     dx, al

    ; read all sector into memory
    .next_sector:
        push    ecx

    ; chck if we need to read
    .try_again:
        mov     dx, 0x1F7
        in      al, dx
        test    al, 8
        jz      .try_again

    ; read 256 words at a time
    mov     ecx, 256
    mov     dx, 0x1F0
    rep     insw
    pop     ecx
    loop    .next_sector
    ; end of reading sectors in memory
    ret

times 510-($-$$) db 0
dw      0xAA55
