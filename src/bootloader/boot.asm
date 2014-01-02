[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x07c0:START                                ; set CS(segment register) to 0x07C0 and jump to START label. 
TOTALSECTORCOUNT:
        dw        0x02
KERNEL32SECTORCOUNT:
        dw        0x02

START:
        mov ax, 0x07c0
        mov ds, ax                                        ; set DS(segment register) to the address of the bootloader. 

mov ax, 0xb800
mov es, ax                                                ; set ES(segment register) to the address of the video memory starting address. 

; stack initialization
mov ax, 0x0000
mov ss, ax
mov sp, 0xfffe
mov bp, 0xfffe

; clear the screen
int 0x10

; print welcome message
push WELCOMEMESSAGE
call PRINTMESSAGE
add sp, 6

; print OS loading message
push OSIMAGELOADINGMESSAGE
call PRINTMESSAGE
add sp, 6


mov ax, 0x00
mov dl, 0x00
int 0x13													; reset disk
jc DISKREADERROR

; load OS image
mov ch, 0x00
mov cl, 0x02
mov dh, 0x00
mov dl, 0x00

mov si, 0x1000
mov es, si                                                  ; set the start address of OS image to 0x10000
mov bx, 0x0000

mov di, word[TOTALREADIMAGE]
LOADOSIMAGE:
        mov ah, 0x02
        mov al, 0x01

        int 0x13
        jc DISKREADERROR

        sub di, 1
        cmp di, 0
        je FINISHLOADOSIMAGE

        add si,        0x020 
        mov es, si

        add cl, 0x01
        cmp cl, 19

        jl LOADOSIMAGE
        mov cl, 0x01

        add dh, 0x01
        cmp dh, 2

        jl LOADOSIMAGE 
        mov dh, 0x00

        add ch, 0x01
        cmp ch, 80

        jl LOADOSIMAGE
        mov ch, 0x00

        jmp LOADOSIMAGE

; print OS loading message
FINISHLOADOSIMAGE:
        push PASSMESSAGE
        call PRINTMESSAGE
        add sp, 6

jmp 0x1000:0x0000                                           ; jump to 2nd stage     

; disk error handling
DISKREADERROR:
        push FAILMESSAGE
        call PRINTMESSAGE

        jmp $

; show welcome message
PRINTMESSAGE:
        push ax
        push dx
        
        mov    ah,9											; Display String Service
        mov    dx,word[bp + 8]    							; Offset of message (Segment DS is the right segment in .COM files)
        int    21h											; call DOS int 21h service to display message at ptr ds:dx

        pop dx
        pop ax
        ret

WELCOMEMESSAGE:
        db 'Welcome to SOS(Simple OS)!', 0
OSIMAGELOADINGMESSAGE:
        db 'OS Image Loading.......................[    ]', 0
PASSMESSAGE:
        db 'Pass', 0
FAILMESSAGE:
        db 'Fail', 0

TOTALREADIMAGE:
        dw 1024

times 510 - ($ - $$) db 0x00								; fill address 0 to 510 with 0

db 0x55                                                     ; set 511 to 0x55
db 0xAA                                                     ; set 512 to 0xAA
