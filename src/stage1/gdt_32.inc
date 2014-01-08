%ifndef __GDT_INC_INCLUDED__
%define __GDT_INC_INCLUDED__

;*********************************
;* Global Descriptor Table (GDT) *
;*********************************
NULL_DESC:
    dd 0            ; null descriptor
    dd 0

CODE_DESC:
    dw 0xFFFF       ; limit low
    dw 0            ; base low
    db 0            ; base middle
    db 10011010b    ; access
    db 11001111b    ; granularity
    db 0            ; base high

DATA_DESC:
    dw 0xFFFF       ; data descriptor
    dw 0            ; limit low
    db 0            ; base low
    db 10010010b    ; access
    db 11001111b    ; granularity
    db 0            ; base high

gdtr:
    Limit dw gdtr - NULL_DESC - 1 ; length of GDT
    Base dd NULL_DESC   ; base of GDT

%endif ;__GDT_INC_INCLUDED__
