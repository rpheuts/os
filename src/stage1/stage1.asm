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
; 16-bit Functions
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

;;;;;;;;;;;;;;;;;;;;;;
; Protected Mode
;;;;;;;;;;;;;;;;;;;;;;
    
%define PAGE_PRESENT    (1 << 0)
%define PAGE_WRITE      (1 << 1)
%define FREE_SPACE 0x9000

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

    mov edi, FREE_SPACE

    ; Build the Page Map Level 4.
    ; es:di points to the Page Map Level 4 table.
    lea eax, [es:di + 0x1000]         ; Put the address of the Page Directory Pointer Table in to EAX.
    or eax, PAGE_PRESENT | PAGE_WRITE ; Or EAX with the flags - present flag, writable flag.
    mov [es:di], eax                  ; Store the value of EAX as the first PML4E.
 
 
    ; Build the Page Directory Pointer Table.
    lea eax, [es:di + 0x2000]         ; Put the address of the Page Directory in to EAX.
    or eax, PAGE_PRESENT | PAGE_WRITE ; Or EAX with the flags - present flag, writable flag.
    mov [es:di + 0x1000], eax         ; Store the value of EAX as the first PDPTE.
 
 
    ; Build the Page Directory.
    lea eax, [es:di + 0x3000]         ; Put the address of the Page Table in to EAX.
    or eax, PAGE_PRESENT | PAGE_WRITE ; Or EAX with the flags - present flag, writeable flag.
    mov [es:di + 0x2000], eax         ; Store to value of EAX as the first PDE.
 
 
    push di                           ; Save DI for the time being.
    lea di, [di + 0x3000]             ; Point DI to the page table.
    mov eax, PAGE_PRESENT | PAGE_WRITE    ; Move the flags into EAX - and point it to 0x0000.

    ; Build the Page Table.
.LoopPageTable:
    mov [es:di], eax
    add eax, 0x1000
    add di, 8
    cmp eax, 0x200000                 ; If we did all 2MiB, end.
    jb .LoopPageTable 

    pop di                            ; Restore DI.

    ; Disable IRQs
    mov al, 0xFF                      ; Out 0xFF to 0xA1 and 0x21 to disable all IRQs.
    out 0xA1, al
    out 0x21, al

    ; Prepare for switch to 64-bit
    mov eax, cr4
    or eax, 0x20                	; set PAE bit of CR4 to 1
    mov cr4, eax

    mov edx, edi                      ; Point CR3 at the PML4.
    mov cr3, edx   
 
    mov ecx, 0xc0000080        		; read MSR register
    rdmsr                                

    or eax, 0x0100                	; write MSR register
    wrmsr

    mov eax, cr0
    or eax, 0xe0000000
    xor eax, 0x60000000
    mov cr0, eax
   
    jmp 0x18:(x8664Mode - $$ + 0x10000)


;;;;;;;;;;;;;;;;;;;;;;
; x86_64 Mode
;;;;;;;;;;;;;;;;;;;;;;

[BITS 64]

x8664Mode:
    mov ax, 0x20
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ss, ax
    mov rsp, 0x6ffff8
    mov rbp, 0x6ffff8
   
    mov	rax, 0x10400
    jmp rax
