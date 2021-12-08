[org 1000h]

[bits 32]

start:
	
	mov esi, message
	call pm_print_string
	
	jmp $
	
	message db "Hello, World! This is your Kernel speaking.", 0
	
	
%include "../Features/pm_strings.asm"