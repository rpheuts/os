[ORG 0x00]
[BITS 16]

global _start
_start:
	jmp Continue			; jump to Continue label. 

%include "gdt.inc"

LOADMESSAGE			db 'Stage 2 initializing...                 ', 0
A20MESSAGE			db 'Enabling A20 Gate...                    ', 0
PMMESSAGE			db 'Switching to Protected mode...          ', 0
X64MESSAGE			db 'Switching to x86_64 mode...             ', 0
FAILMESSAGE			db 'Error', 0

Continue:
	mov ax, 0x1000
	mov ds, ax			; set DS to the address of the stage1 code. 

	;mov ax, 0xb800
	;mov es, ax			; set ES to address of the video memory starting address. 

	; stack initialization
	mov ax, 0x0000
	mov ss, ax
	mov sp, 0xfffe
	mov bp, 0xfffe

	; print OS loading message
	mov si, LOADMESSAGE
	call DisplayMessage
	
	; print A20 Gate message
	mov si, A20MESSAGE
	call DisplayMessage
	
	cli                     	; Disable interrupts
	
	call OpenA20Gate
	
	mov si, PMMESSAGE
	call DisplayMessage

	mov ax, 0x03
	int 10h
	
	lgdt [GDTR]             	; Load GDT from GDTR (see gdt_32.inc)
    	mov eax, 0x4000003B         	; PG=0, CD=1, NW=0, AM=0, WP=0, NE=1, ET=1, TS=1, EM=0, MP=1, PE=1
	mov cr0, eax

    	jmp dword 0x08:(ProtectedMode - $$ + 0x10000)
	
		
;;;;;;;;;;;;;;;;;;;;;;
; FUNCTIONS
;;;;;;;;;;;;;;;;;;;;;;

DisplayMessage:
	lodsb					; load next character
	or al, al				; test for NUL character
	jz DisplayMessageDone
	
	mov ah, 0x0E			; BIOS teletype
    mov bh, 0x00			; display page 0
	mov bl, 0x07			; text attribute
    int 0x10				; invoke BIOS
    
    jmp DisplayMessage
DisplayMessageDone:
    ret
    
ErrorMessage:
	mov si, FAILMESSAGE
	call DisplayMessage
	jmp $

OpenA20Gate:
	mov ax, 0x2401
    int 0x15

    jc A20GATEERROR
    jmp A20GATESUCCESS

A20GATEERROR:
    in al, 0x92

    or al, 0x02
    and al, 0xFE

    out 0x92, al

A20GATESUCCESS:	
	ret
    
[BITS 32]    
ProtectedMode:
    mov ax, 0x10			; set DS to 0x10 (Data Segment Descriptor)
    mov ds, ax
    mov ss, ax				; set stack register(SS, ESP, EBP)
    mov esp, 0xfffe
    mov ebp, 0xfffe
	
    mov	ax, 0x08
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    jmp dword 0x08:0x10200	
	
times 512 - ($ - $$) db 0x00			; fill address 0 to 512 with 0
