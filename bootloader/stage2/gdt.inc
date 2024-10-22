;**********************************************************
;       gdp.inc
;               - GDT (Global Descriptor Table) routines
;
;       AlphaOS
;**********************************************************

%ifndef __GDT_INC__
%define __GDT_INC__

bits    16

;**********************************************************
;       InstallGDT()
;               - Install the GDT
;**********************************************************

InstallGDT:
    cli                                         ; clear interrupts
    pusha                                       ; save registers
    lgdt    [toc]                               ; load GDT
    sti                                         ; restore interrupts
    popa                                        ; restore registers
    ret                                         ; return

;**********************************************************
;       GLOBAL DESCRIPTOR TABLE (GDT)
;**********************************************************

gdt_data:
    dd      0                                   ; null descriptor
    dd      0

; gdt code:
    dw      0FFFFH                              ; limit low
    dw      0                                   ; base low
    db      0                                   ; base middle
    db      10011010b                           ; access
    db      11001111b                           ; granularity
    db      0                                   ; base high

; gdt data:
    dw      0FFFFH                              ; limit low
    dw      0                                   ; base low
    db      0                                   ; base middle
    db      10010010b                           ; access ; only difference with gdt_code
    db      11001111b                           ; granularity
    db      0                                   ; base high

end_of_gdt:
toc:
    dw      end_of_gdt - gdt_data - 1           ; limit (size of the GDT)
    dd      gdt_data                            ; base of the GDT

;**********************************************************
;       Explicite descriptor offset names
;**********************************************************

%define NULL_DESC 0
%define CODE_DESC 0x8
%define DATA_DESC 0x10

%endif  ; __GDT_INC__
