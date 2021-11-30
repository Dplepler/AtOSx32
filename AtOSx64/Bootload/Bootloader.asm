
[bits 16]
bootload_start:

	cli 						; Disable interrupts: There are no interrupts in protected mode

	lgdt [GDT_DESCRIPTOR] 		; Load Global Descriptor Table onto the CPU


	; Set the first bit of the cr0 register to 1, telling the CPU we
	; would like to switch to protected mode
	mov eax, cr0 	 		; cr0 is a special control register that can tell the CPU we are using protected mode
	or eax, 00000001b		; We cannot set cr0's last bit to 1 directly, so we use a general purpose register
	mov cr0, eax			; Also, we do not want to change previous values of cr0 

	
	; After telling the CPU we want to use protected mode,
	; there is a risk that it will use the pipeline mode to process instructions instead
	; A way to avoid that is to clear the pipeline by far jumping to another segment 
	; This will automatically set CS to our code segment + clear the pipeline
	
	jmp CODE_SEG:init_protected_mode	


[bits 32]
init_protected_mode:

	; Set all segment registers to the beginning of the data segment
	; Note that CS will already be set to the code segment after the far jump to here
	mov ax, DATA_SEG
	mov ds, ax
	mov cs, ax
	mov ss, ax
	mov es, ax
	
	; Set EBP and ESP to the top of the stack
	mov ebp, 90000h
	mov esp, ebp
	
	
finish:

	jmp $ 		; Jump to the current address (Hang)


times 510-($-$$) db 0	; Fill memory with 0 so that the size of the bootloader will always be 512 (+2 signature bytes)
dw 0AA55h				; Boot sector signature - tell the CPU this is our bootloader				


;=====================================
;			     INCLUDES				     
;=====================================

	%INCLUDE "GDT.asm"