[org 7C00h] 				; Set location counter
 
KERNEL_OFFSET equ 1000h 	; Location of our kernel in memory

; Start of the boot sector's main routine
bootload_start:
	
	mov [BOOT_DRIVE], dl 	; Save boot device number stored in dl by BIOS
	
	; Set up stack
	mov bp, 9000h
	mov sp, bp
	
	call load_kernel
	call switch_to_pm

	jmp $ 			; Hang
	


; Includes
%include "Bootload/GDT.asm" 						; Global descriptor table
%include "Bootload/protected_mode_setup.asm"		; Routines to set up and initialize protected mode
%include "Features/pm_strings.asm"					; String features in 32 bit protected mode

[bits 16]

; Load the kernel in 16 bit real mode
load_kernel:

	; Set up parameters to load disk with
	
	mov ebx, KERNEL_OFFSET
	mov dl, [BOOT_DRIVE]		; Boot device number 
	mov dh, 15					; Amount of sectors to load
	call load_disk				; Call routine

	ret

	
; load DH sectors to ES:BX from drive DL
load_disk:

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
	

[bits 32]

; Beginning of protected mode! How cool
genesis:

	call 0C0000000h 		; Call the virtual address of the Kernel
	jmp $					; Hang at the end of the kernel
	


; Variables
BOOT_DRIVE			db 0 							; Save boot device number
disk_error_message 	db "Disk read error!" , 0
success_message    	db "Successfully switched to protected mode! DEBUG WOW!", 0
	
	

times 510-($-$$) db 0	; Fill memory with 0 so that the size of the bootloader will always be 512 (+2 signature bytes)
dw 0AA55h				; Boot sector signature - tell the CPU this is our bootloader