
[bits 16]

; Switches from real mode to protected mode
switch_to_pm:

	cli 						; Disable interrupts: There are no BIOS interrupts in protected mode

	lgdt [GDT_DESCRIPTOR] 		; Load Global Descriptor Table onto the CPU


	; Set the first bit of the cr0 register to 1, telling the CPU we
	; would like to switch to protected mode
	mov eax, cr0 	 		; cr0 is a special control register that can tell the CPU we are using protected mode
	or 	eax, 00000001b		; We cannot set cr0's last bit to 1 directly, so we use a general purpose register
	mov cr0, eax			; Also, we do not want to change previous values of cr0 



	; After telling the CPU we want to use protected mode,
	; there is a risk that it will use the pipeline mode to process instructions instead
	; A way to avoid that is to clear the pipeline by far jumping to another segment 
	; This will automatically set CS to our code segment + clear the pipeline
	
	jmp CODE_SEG:init_protected_mode



[bits 32]

PD_OFFSET equ 3000h 	; Table directory entry offset
PT_OFFSET equ 4000h		; First page table offset

init_protected_mode:


	; Set all segment registers to the beginning of the data segment
	; Note that CS will already be set to the code segment after the far jump to here
	mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; Set EBP and ESP to the top of the stack
	mov ebp, 90000h
	mov esp, ebp
      
    ; Set all directory entries to not present, with read/write access
    ; The page directory consists of 1024 entries, each 32 bit
	mov eax, 2h	
	mov edi, PD_OFFSET
	mov ecx, 1024

	rep stosd  			; Repeat copying EAX's value to EDI memory location ECX times

	xor ecx, ecx
	mov edi, PT_OFFSET


; Map the first page table's entries to physical page frames
; In this case, as of now, we map the pages into the beginning of physical memory
.fill_table:

	mov eax, 1000h 		; Each page table entry maps 4 kilobytes of data, so we would give the 4k aligned offset each time

	mul ecx 			; Index times 4096 (4k) bytes
	or eax, 3 			; Set present flag to true, as well as read/write

	mov dword [edi], eax	; Map page frame value to entry
	add edi, 4  			; Go to next entry
	inc ecx 				
	cmp ecx, 1024 			; We only want to map 1024 entries (that make up a page table)

	jle .fill_table

	mov edi, PD_OFFSET 			 		
	mov dword [edi], PT_OFFSET	; Put the first page table into the first page directory entry 
	or dword [edi], 3 			; Turn present flag on

	mov eax, TD_OFFSET 		; The address that points to the directory table, 1024 entries, 32 bits each
	mov cr3, eax			; We put the address of our directory table in the cr3 register

	
	mov eax, cr0			; To enable paging we need to set the correct flags in the cr0 register
	or 	eax, 80000000h
	mov cr0, eax

	jmp genesis 			; Go back to the bootloader to start executing the kernel!
			