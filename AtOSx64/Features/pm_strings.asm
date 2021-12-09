
[bits 32]

VIDEO_MEMORY equ 0B8000h 	; Video memory starts at this address
WHITE_ON_BLACK equ 0Fh		; White characters on black screen attribute

; pm_print_string prints data starting from offset passed in ESI, until 0 is read
pm_print_string:

	pusha 				
	mov edx, VIDEO_MEMORY 		; Get video memory address to print to
	
.repeat:

	lodsb 						; Load byte from ESI to al
	mov ah, WHITE_ON_BLACK		; Print white character on black screen
	
	cmp al, 0 					; If data read was 0 - finish procedure
	je .done
	
	mov [edx], ax 				; Store the character and it's attributes onto screen
	
	add edx, 2 					; Increase video memory to print to the next cell
	
	jmp .repeat					; Repeat for all characters
	
.done:

	popa
	ret
	
	