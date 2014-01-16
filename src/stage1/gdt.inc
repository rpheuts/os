%ifndef __GDT_INC_INCLUDED__
%define __GDT_INC_INCLUDED__

;*********************************
;* Global Descriptor Table (GDT) *
;*********************************
GDTR:
        dw        GDTEND - GDT - 1                ; size of GDT
        dd        (GDT - $$ + 0x10000)        ; 32-bit base address

GDT:
        NULLDESCRIPTOR:
                dw        0x0000
                dw        0x0000
                db        0x00  
                db        0x00  
                db        0x00
                db        0x00

        CODESEGMENTDESCRIPTOR:
                dw        0xffff        ; segment size(15:00)
                dw        0x0000        ; base address(15:00)
                db        0x00        ; base address(23:16)
                db        0x9a        ; P=1, DPL=0, S=1, Type=0x0a(Execute/Read)
                db        0xcf        ; G=1, D/B=1, L=0, AVL=0, segment size(19:16)
                db        0x00        ; base address(31:24)

        DATASEGMENTDESCRIPTOR:
                dw        0xffff        ; segment size(15:00)
                dw        0x0000        ; base address(15:00)
                db        0x00        ; base address(23:16)
                db        0x92        ; P=1, DPL=0, S=1, Type=0x02(Read/Write)
                db        0xcf        ; G=1, D/B=1, L=0, AVL=0, segment size(19:16)
                db        0x00        ; base address(31:24)
	
	x64_CODESEGMENTDESCRIPTOR:
                dw        0xffff        ; segment size(15:00)
                dw        0x0000        ; base address(15:00)
                db        0x00        ; base address(23:16)
                db        0x9a        ; P=1, DPL=0, S=1, Type=0x0a(Execute/Read)
                db        0xaf        ; G=1, D/B=0, L=1, AVL=0, segment size(19:16)
                db        0x00        ; base address(31:24)

        x64_DATASEGMENTDESCRIPTOR:
                dw        0xffff        ; segment size(15:00)
                dw        0x0000        ; base address(15:00)
                db        0x00        ; base address(23:16)
                db        0x92        ; P=1, DPL=0, S=1, Type=0x02(Read/Write)
                db        0xaf        ; G=1, D/B=0, L=1, AVL=0, segment size(19:16)
                db        0x00        ; base address(31:24)
GDTEND:
	
%endif ;__GDT_INC_INCLUDED__
