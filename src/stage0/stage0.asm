[ORG 0x00]
[BITS 16]

	jmp 0x07c0:Start			; set CS(segment register) to 0x07C0 and jump to START label. 

TOTALSECTORCOUNT		dw 0x02
KERNEL32SECTORCOUNT		dw 0x02

LOADMESSAGE			db 'Loading Stage 2...', 0
PASSMESSAGE			db 'Pass', 0
FAILMESSAGE			db 'Fail', 0

Start:
	mov ax, 0x07c0
	mov ds, ax				; set DS to the address of the bootloader. 

	mov ax, 0xb800
	mov es, ax				; set ES to address of the video memory starting address. 

	; stack initialization
	mov ax, 0x0000
	mov ss, ax
	mov sp, 0xfffe
	mov bp, 0xfffe

	; Clear Screen
	int 10h

	; print OS loading message
	mov si, LOADMESSAGE
	call DisplayMessage
		
	; reset disk
	mov ax, 0x00
	mov dl, 0x00
	int 0x13
	jc DiskError
	
	; read sectors from disk	
	mov ax, 0x1000
	mov es, ax				; segment = 0x1000
	xor bx, bx				; offset = 0
	
	mov ah, 0x02				; BIOS function 2h
	mov al, 0x20				; Sectors to read = 32
	mov ch, 0x00				; Track = 0
	mov cl, 0x02				; Sector = 2
	mov dh, 0x00				; Head = 0
	mov dl, 0x80				; HDD 1
	
	int 0x13				; Perform read
	jc DiskError
	
	jmp 0x1000:0x0000
		
;;;;;;;;;;;;;;;;;;;;;;
; FUNCTIONS
;;;;;;;;;;;;;;;;;;;;;;

DiskError:
        mov si, FAILMESSAGE
        call DisplayMessage
        jmp $

DisplayMessage:
          lodsb					; load next character
          or al, al				; test for NUL character
          jz .DONE
          mov ah, 0x0E				; BIOS teletype
          mov bh, 0x00				; display page 0
          mov bl, 0x07				; text attribute
          int 0x10				; invoke BIOS
          jmp DisplayMessage
     .DONE:
          ret

times 510 - ($ - $$) db 0x00			; fill address 0 to 510 with 0

db 0x55                                                           ; set 511 to 0x55
db 0xAA                                                           ; set 512 to 0xAA
