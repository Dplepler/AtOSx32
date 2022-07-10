[org 7E00h] 										; Load right after the stage0 boot sector
[bits 16]
INIT_KERNEL_OFFSET equ 1000h 		; Location of our kernel in memory at first
PHYS_KERNEL_OFFSET equ 100000h	; Location of our kernel in memory

; Start of the boot sector's main routine
bootload_start:
	
	; Set segment and stack
	xor ax, ax       	
	mov ds, ax          
	mov ss, ax          
	mov sp, 9c00h

	call enable_a20
	call unreal_mode
	call load_kernel
	call switch_to_pm

	jmp $ 	; Hang
	

; Enables A20 address line to access higher memory in 16 bit
enable_a20:

	pusha

	; Don't activate A20 address line if it was already activated (most likely if running on QEmu)
	call check_a20
	cmp ax, 1
	je .exit

	; Activate A20 address line
	mov ax, 2401h               
	int 15h


.exit:

	popa
	ret

; Checks if the A20 line is enabled
; Output: AX = 1 if A20 line is enabled, otherwise 0
check_a20:

	pushf 	; Save flag register

	; Save registers we are going to use
	push ds
	push es
	push di
	push si

	cli 				; Disable interupts

	xor ax, ax 	; Reset AX
	mov es, ax	; Set ES to 0

	not ax		
	mov ds, ax	; Set DS to 0xFFFF


	; We are now going to check if our boot signature (0AA55h located at 0000:7DFE) is equal to 
	; the value stored at memory FFFF:7E0E
	; If it is, it means that the memory is wrapped around and that the A20 address line is disabled
	
	mov di, 500h
	mov si, 510h

	mov al, byte [es:di]
	push ax
 
	mov al, byte [ds:si]
	push ax

	mov byte [es:di], 0h			; Make sure value is not 0xFF
	mov byte [ds:si], 0FFh

	cmp byte [es:di], 0FFh		; If this was changed to 0xFF then there is a wrap around

	pop ax
	mov byte [ds:si], al

	pop ax
	mov byte [es:di], al

	xor ax, ax
	je .exit

	mov ax, 1	

.exit:

	pop si
	pop di
  pop es
  pop ds
  popf
	
	ret

; Switch to unreal mode so we can load a big big kernel
; This will not be documented well since most operations are documented in the protected mode setup file
unreal_mode:    

	cli 		; Clear interrupts
	push ds		; Save segment

	lgdt [GDT_DESCRIPTOR]	; Load Global Descriptor Table

	; Temporarily go to protected mode
	mov eax, cr0          
  or al, 1                
  mov cr0, eax

	jmp .continue

.continue:

	mov  bx, 0x08        ; Select descriptor 1
	mov  ds, bx          ; 8h = 1000b

	; Back to real mode
	and al, 0FEh
	mov cr0, eax

	pop ds      ; Get back old segment
	sti					; Return interrupts

	ret


; 				INCLUDES
;===========================================;
%include "Bootload/GDT.asm" 				;		Global descriptor table
%include "Bootload/DAP.asm"					;		Disk address packet
%include "Bootload/protected_mode_setup.asm";		Routines to set up and initialize protected mode			;		String features in 32 bit protected mode
;===========================================;

[bits 16]

; Load the kernel in 16 bit real mode
load_kernel:

	; Check if BIOS supports extention interrupts before using one
	mov ah, 41h
	mov bx, 55AAh
	mov dl, 80h
	int 13h
	jc .no_extentions


	mov dl, 80h					  ; Boot device number - First HDD
	mov si, DAP_START			; SI will hold the Disk Address Packet offset
	call load_disk

	ret

.no_extentions:

	mov si, no_extentions_message
	call print_string
	jmp $

	
; load DH sectors to ES:BX from drive DL
load_disk:

	mov ah, 42h	 		; BIOS read sector extended function  
	int 13h 			  ; BIOS interrupt

	jc .disk_error 	; Jump if error (i.e. carry flag set)
	
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

; Jump to our kernel!
genesis:

	call 0C0000000h 		; Call the virtual address of the Kernel
	jmp $					; Hang at the end of the kernel
	


; Variables
BOOT_DRIVE				      db 0 	; Save boot device number
disk_error_message 		  db    "Disk read error!" , 0
no_extentions_message 	db    "BIOS does not support extended interrupts", 0
	
