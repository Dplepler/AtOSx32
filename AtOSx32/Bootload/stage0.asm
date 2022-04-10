[org 7C00h] 		; Set location counter
[bits 16]

BOOT_OFFSET equ 1000h

stage0_start:

    mov [BOOT_DRIVE], dl    ; Save boot device number stored in DL by BIOS

    ; Set up stack
	mov bp, 9000h
	mov sp, bp
    
    call load_stage1

    call BOOT_OFFSET
    jmp $


; Load the kernel in 16 bit real mode
load_stage1:

	; Set up parameters to load disk with
	
	mov bx, BOOT_OFFSET
	mov dl, [BOOT_DRIVE]		; Boot device number 
	mov dh, 1					; Amount of sectors to load
	call load_boot				; Call routine

	ret

	
; load DH sectors to ES:BX from drive DL
load_boot:

	push dx 									
	mov ah, 2h	 		; BIOS read sector function
	mov al, dh 			; Read DH sectors
	mov ch, 0 			; Select cylinder 0
	mov dh, 0 			; Select head 0
	mov cl, 2 			; Start reading from second sector (i.e after the boot sector)
	int 13h 			; BIOS interrupt
	jc .disk_error 		; Jump if error (i.e. carry flag set)
	pop dx 				; Restore DX from the stack
	cmp dh , al 		; if AL (sectors read) != DH (sectors expected)
	jne .disk_error 	; display error message
	ret
	
.disk_error:

	; Print error
	mov si, disk_error_message
	call print_string
	jmp $ 						; Hang
	
	
; Output string in SI to screen	
print_string:	

	pusha
	mov ah, 0Eh			; int 10h teletype function

.repeat:

	lodsb				; Get char from string
	cmp al, 0
	je .done			; If char is zero, end of string
	int 10h				; Otherwise, print it
	jmp short .repeat 	; And repeat for all characters

.done:

	popa
	ret

BOOT_DRIVE			db 0 							; Save boot device number
disk_error_message 	db "Disk read error!" , 0

times 510-($-$$) db 0	; Fill memory with 0 so that the size of the bootloader will always be 512 (+2 signature bytes)
dw 0AA55h				; Boot sector signature - tell the CPU this is our bootloader