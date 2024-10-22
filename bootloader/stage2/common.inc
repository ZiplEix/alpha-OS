;**********************************************************
;       commun.inc
;               - Common definitions
;
;       AlphaOS
;**********************************************************

%ifndef __COMMUN_INC__
%define __COMMUN_INC__

; where the kernel is to be loaded in pmode
%define IMAGE_PMODE_BASE 0x100000

; where the kernel is to be loaded in rmode
%define IMAGE_RMODE_BASE 0x3000

; kernel name (Must be 11 bytes)
ImageName   db "KERNEL  SYS",

; size of kernel image in bytes
ImageSize   db 0

%endif ; __COMMUN_INC__
