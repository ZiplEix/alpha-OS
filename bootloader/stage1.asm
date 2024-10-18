;***************************************************
; Bootloader - stage 1
;    - Load the second stage of the bootloader
;    - Jump to the second stage
;
;      - By Baptiste Leroyer
;***************************************************

bits 16

org 0

start:
    jmp main

;*********************************************
;    BIOS Parameter Block
;*********************************************

bpbOEM                  db "Alpha OS"            ; OEM identifier (Cannot exceed 8 bytes!)
bpbBytesPerSector:      DW 512
bpbSectorsPerCluster:   DB 1
bpbReservedSectors:     DW 1
bpbNumberOfFATs:        DB 2
bpbRootEntries:         DW 224
bpbTotalSectors:        DW 2880
bpbMedia:               DB 0xf8  ;; 0xF1
bpbSectorsPerFAT:       DW 9
bpbSectorsPerTrack:     DW 18
bpbHeadsPerCylinder:    DW 2
bpbHiddenSectors:       DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber:          DB 0
bsUnused:               DB 0
bsExtBootSignature:     DB 0x29
bsSerialNumber:         DD 0xa0a1a2a3
bsVolumeLabel:          DB "MOS FLOPPY "
bsFileSystem:           DB "FAT12   "

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
; Read a series of sectors from the disk
; CX = number of sectors to read
; AX = starting sector
; ES:BX = buffer to read to
;***************************************************

ReadSectors:
    .MAIN:
        mov     di, 0x0005                           ; Number of retries (5)
    .SECTORLOOP:
        push    ax
        push    bx
        push    cx
        call    LBACHS                              ; Convert logical block to CHS
        mov     ah, 0x02                            ; BIOS read sector function
        mov     al, 0x01                            ; Read one sector
        mov     ch, BYTE [absoluteTrack]            ; track
        mov     cl, BYTE [absoluteSector]           ; sector
        mov     dh, BYTE [absoluteHead]             ; head
        mov     dl, BYTE [bsDriveNumber]            ; drive
        int     0x13                                ; BIOS interrupt
        jnc     .SUCCESS                            ; If read was successful, continue
        xor     ax, ax                              ; Otherwise, reset disk system
        int     0x13                                ; and try again
        dec     di                                  ; If retry count isn't zero, try again
        pop     cx
        pop     bx
        pop     ax
        jnz     .SECTORLOOP                         ; If retry count isn't zero, try again
        int     0x18                                ; If we're out of retries, print error message and halt
    .SUCCESS:
        mov     si, msgProgress
        call    Print
        pop     cx
        pop     bx
        pop     ax
        add     bx, WORD [bpbBytesPerSector]        ; Move buffer to next sector
        inc     ax                                  ; Move to next sector
        loop    .MAIN                               ; If there are more sectors to read, read them
        ret

;***************************************************
; Convert CHS to LBA
; LBA = (cluster - 2) * sectors per cluster
;***************************************************

ClusterLBA:
    sub     ax, 0x0002                              ; zero based sector number
    xor     cx, cx
    mov     cl, BYTE [bpbSectorsPerCluster]         ; converte byte to word
    mul     cx                                      ; AX = AX * CX
    add     ax, WORD [datasector]                   ; base data sector
    ret

;***************************************************
; Convert LBA (logical block) to CHS
; AX=>LBA Address to convert
;
; absolute sector = (logical sector / sectors per track) + 1
; absolute head   = (logical sector / sectors per track) MOD number of heads
; absolute track  = logical sector / (sectors per track * number of heads)
;***************************************************

LBACHS:
    xor     dx, dx                                  ; prepare dx:ax for operation
    div     WORD [bpbSectorsPerTrack]               ; calculate ax / SectorsPerTrack
    inc     dl                                      ; adjust for sector 0
    mov     BYTE [absoluteSector], dl               ; save the sector
    xor     dx, dx                                  ; prepare dx for operation
    div     WORD [bpbHeadsPerCylinder]              ; calculate ax / bpbHeadsPerCylinder
    mov     BYTE [absoluteHead], dl                 ; head number
    mov     BYTE [absoluteTrack], al                ; track number
    ret

;***************************************************
; Bootloader Entry Point
;***************************************************

main:
    ;---------------------------------------------------
    ; code located at 000:7C00, adjust segment registers
    ;---------------------------------------------------
    cli                                             ; disable interrupts
    mov     ax, 0x07C0                              ; set up registers to point to our segment
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    ;---------------------------------------------------
    ; create stack
    ;---------------------------------------------------
    mov    ax, 0x0000                               ; set up stack
    mov    ss, ax
    mov    sp, 0xFFFF
    sti                                             ; restore interrupts

    ;---------------------------------------------------
    ; Display loading message
    ;---------------------------------------------------
    mov     si, msgLoading
    call    Print

    ;---------------------------------------------------
    ; Load root directory table
    ;---------------------------------------------------
    LOAD_ROOT:
        ; compite size of root directory and store in cx
        xor     cx, cx
        xor     dx, dx
        mov     ax, 0x0020                          ; Root directory has 32 entries
        mul     WORD [bpbRootEntries]               ; 32 * 224 = 7168 -> total size of directory
        div     WORD [bpbBytesPerSector]            ; 7168 / 512 = 14 sectors -> sectors used by root directory
        xchg    ax, cx                              ; store the number of sectors in cx

        ; compute location of root directory and store in ax
        mov     al, BYTE [bpbNumberOfFATs]          ; number of FATs
        mul     WORD [bpbSectorsPerFAT]             ; 2 * 9 = 18 -> total sectors used by FAT
        add     ax, WORD [bpbReservedSectors]       ; 18 + 1 = 19 -> total sectors used by FAT + reserved sectors
        mov     WORD [datasector], ax               ; store the location of root directory in datasector
        add     WORD [datasector], cx

        ; read root directory into memory at 7C00:0200
        mov     bx, 0x0200                          ; copy root dir above bootcode
        call    ReadSectors

    ;---------------------------------------------------
    ; Find stage 2
    ;---------------------------------------------------
        ; browse root directory for binary image
        mov     cx, WORD [bpbRootEntries]           ; load loop counter with number of root directory entries
        mov     di, 0x0200                          ; point di to first root entry

    .LOOP:
        push    cx
        mov     cx, 0x000B                          ; compare first 11 bytes of filename
        mov     si, ImageName
        push    di
        rep     cmpsb                               ; compare filename to test entry match
        pop     di
        je      LOAD_FAT                            ; if match, load file
        pop     cx
        add     di, 0x0020                          ; move to next entry
        loop    .LOOP                               ; loop through all entries
        jmp     FAILURE                             ; if no match, print error message and halt

    ;---------------------------------------------------
    ; Load FAT
    ;---------------------------------------------------
    LOAD_FAT:
        ; save starting cluster of boot image
        ; mov     si, msgCRLF
        ; call    Print
        mov     dx, WORD [di + 0x001A]              ; get starting cluster number
        mov     WORD [cluster], dx                  ; file's first cluster

        ; compute size of FAT and store in cx
        xor     ax, ax
        mov     al, BYTE [bpbNumberOfFATs]          ; number of FATs
        mul     WORD [bpbSectorsPerFAT]             ; total sectors used by FAT
        mov     cx, ax                              ; store the size of FAT in cx

        ; compute location of FAT and store in ax
        mov     ax, WORD [bpbReservedSectors]       ; reserved sectors

        ; read Fat into memory at 7C00:0200
        mov     bx, 0x0200                          ; copy FAT above bootcode
        call    ReadSectors

        ; read image file into memory at 0050:0000
        ; mov     si, msgCRLF
        ; call    Print
        mov     ax, 0x0050
        mov     es, ax                              ; destination for image
        mov     bx, 0x0000                          ; copy image to 0050:0000
        push    bx

    ;---------------------------------------------------
    ; Load Stage 2
    ;---------------------------------------------------
    LOAD_IMAGE:
        mov     ax, WORD [cluster]                  ; cluster to read
        pop     bx                                  ; buffer to read into
        call    ClusterLBA                          ; convert cluster to LBA
        xor     cx, cx
        mov     cl, BYTE [bpbSectorsPerCluster]     ; sectors to read
        call    ReadSectors
        push    bx

        ; compute next cluster
        mov     ax, WORD [cluster]                  ; identify current cluster
        mov     cx, ax                              ; copy current cluster
        mov     dx, ax                              ; copy current cluster
        shr     dx, 0x0001                          ; divide by two
        add     cx, dx                              ; sum for (3/2)
        mov     bx, 0x0200                          ; location of FAT in memory
        add     bx, cx                              ; index into FAT
        mov     dx, WORD [bx]                       ; read two bytes from FAT
        test    ax, 0x0001
        jnz     .ODD_CLUSTER

        .EVEN_CLUSTER:
            and     dx, 0000111111111111b           ; take low twelve bits
            jmp     .DONE

        .ODD_CLUSTER:
            shr     dx, 0x0004                      ; take high twelve bits

        .DONE:
            mov     WORD [cluster], dx              ; store new cluster
            cmp     dx, 0x0FF0                      ; test for end of file
            jb      LOAD_IMAGE

    DONE:
        mov     si, msgCRLF
        call    Print
        push    WORD 0x0050
        push    WORD 0x0000
        retf

    FAILURE:
        mov     si, msgFailure
        call    Print
        mov     ah, 0
        int     0x16                                ; await keypress
        int     0x19                                ; warm boot

    absoluteSector db 0x00
    absoluteHead   db 0x00
    absoluteTrack  db 0x00

    datasector  dw 0x0000
    cluster     dw 0x0000
    ImageName   db "KRNLDR  SYS"
    msgLoading  db 0x0D, 0x0A, "Loading Boot Image ", 0x0D, 0x0A, 0x00
    msgCRLF     db 0x0D, 0x0A, 0x00
    msgProgress db ".", 0x00
    msgFailure  db 0x0D, 0x0A, "MISSING OR CURRUPT KRNLDR. Press Any Key to Reboot", 0x0D, 0x0A, 0x00

    times 510-($-$$) db 0
    dw 0xAA55
