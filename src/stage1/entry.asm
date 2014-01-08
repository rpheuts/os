[ORG 0x00]
[BITS 16]

global _start
_start:
	jmp Continue		; jump to Continue label. 

LOADMESSAGE			db 'Stage 2 initializing...                 ', 0
A20MESSAGE			db 'Enabling A20 Gate...                    ', 0
PMMESSAGE			db 'Switching to Protected mode...          ', 0
FAILMESSAGE			db 'Error', 0

%include "gdt_32.inc"

Continue:
	mov ax, 0x1000
	mov ds, ax				; set DS to the address of the stage1 code. 

	;mov ax, 0xb800
	;mov es, ax				; set ES to address of the video memory starting address. 

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
	
	cli                     ; clear interrupts
	
	call OpenA20Gate
	call CheckA20Gate
	cmp ax, 1
	jne ErrorMessage
	
	mov si, PMMESSAGE
	call DisplayMessage
	
	lgdt [gdtr]             ; load GDT from GDTR (see gdt_32.inc)
	call EnablePMode       	; jumps to ProtectedMode
	
	jmp $					; Just in case it fails
		
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
	in al, 0x93				; switch A20 gate via fast A20 port 92
	
	or al, 2				; set A20 Gate bit 1
	and al, ~1				; clear INIT_NOW bit
	out 0x92, al
	
	ret

CheckA20Gate:
    pushf
    push ds
    push es
    push di
    push si
 
    cli
 
    xor ax, ax 				; ax = 0
    mov es, ax
 
    not ax 					; ax = 0xFFFF
    mov ds, ax
 
    mov di, 0x0500
    mov si, 0x0510
 
    mov al, byte [es:di]
    push ax
 
    mov al, byte [ds:si]
    push ax
 
    mov byte [es:di], 0x00
    mov byte [ds:si], 0xFF
 
    cmp byte [es:di], 0xFF
 
    pop ax
    mov byte [ds:si], al
 
    pop ax
    mov byte [es:di], al
 
    mov ax, 0
    je CheckA20GateDone
 
    mov ax, 1
 
CheckA20GateDone:
    pop si
    pop di
    pop es
    pop ds
    popf
 
    ret
    
EnablePMode:
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    mov        ax, 0x20                                                                ; set DS, ES, FS, GS to 0x10(Data Segment Descriptor)
    mov        ds, ax
	mov es, ax
    mov fs, ax
    mov gs, ax

    mov        ss, ax                                                                        ; set stack register(SS, ESP, EBP)
    mov        esp, 0xfffe
    mov        ebp, 0xfffe

    jmp $
	
	
times 512 - ($ - $$) db 0x00			; fill address 0 to 512 with 0

Next: