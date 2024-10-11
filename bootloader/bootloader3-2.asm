;***************************************************
; bootloader for Intel/AMD x86 CPUs
;
; This bootloader loads the second sector from the floppy
; and then jumps to execute it
;
; Sector 2
;
;      - By Baptiste Leroyer
;***************************************************

; End of sector 1, beginning of sector 2 ---------------------------------

org 0x1000

; cli                           ; just halt the system
; hlt
