[ORG 0x00]
[BITS 16]

	jmp Start			; jump to START label. 

TOTALSECTORCOUNT		dw 0x02
KERNEL32SECTORCOUNT		dw 0x02

LOADMESSAGE			db 'Stage 2 initializing...', 0
PASSMESSAGE			db 'Pass', 0
FAILMESSAGE			db 'Fail', 0

Start:
	mov ax, 0x1000
	mov ds, ax				; set DS to the address of the stage1 code. 

	;mov ax, 0xb800
	;mov es, ax				; set ES to address of the video memory starting address. 

	; stack initialization
	mov ax, 0x0000
	mov ss, ax
	mov sp, 0xfffe
	mov bp, 0xfffe

	; Clear Screen
	;int 10h

	; print OS loading message
	mov si, LOADMESSAGE
	call DisplayMessage
		
	jmp $
		
;;;;;;;;;;;;;;;;;;;;;;
; FUNCTIONS
;;;;;;;;;;;;;;;;;;;;;;

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

