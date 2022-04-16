
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

; These offsets must be 4k aligned
;=======================;
PD_OFFSET  equ 1000000h	; 	Table directory entry offset
PT_OFFSET  equ 1001000h	;	First page table offset
KPT_OFFSET equ 1002000h ;	Kernel page table offset
;=======================;

KERNEL_ENTRY_OFFSET equ 300h 	; Entry in the page directory for our higher half kernel at 0xC0000000

init_protected_mode:

	call relocate_kernel 		; Before we start, relocate the kernel beyond 1MB
	
	; Set all segment registers to the beginning of the data segment
	; Note that CS will already be set to the code segment after the far jump to here
	mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; Setup stack
	mov ebp, 90000h
	mov esp, ebp
      
    ; Set all directory entries to not present, with read/write access
    ; The page directory consists of 1024 entries, each 32 bit
	mov eax, 2h	
	mov edi, PD_OFFSET
	mov ecx, 1024

	rep stosd  			; Repeat copying EAX's value to EDI memory location ECX times

	; Map the last directory entry to itself, this way virtual address 0xFFFFF000 will point to our directory table
	mov edi, 1000FFCh
	mov dword [edi], PD_OFFSET
	or dword [edi], 3

	; To let the bootloader continue without failing, we will identity map the first 4 Megabytes (i.e first page table)
	mov edi, PT_OFFSET		; Some page table offset
	xor ecx, ecx			; Beginning of physical memory
	xor edx, edx			; First directory entry

	call fill_table

	; Now let's map the kernel to make it a higher half kernel
	mov edi, KPT_OFFSET  			; Page table to fill
	mov ecx, 100h					; Physical page index where the kernel is located
	mov edx, KERNEL_ENTRY_OFFSET  	; Kernel's page directory index (Will point to 0xC0000000)

	call fill_table

	; Change the last page entry to point to our VGA buffer
	add edi, 0FFCh				; Add 1023 * 4 to page offset, point to last table entry
	mov dword [edi], 0B8003h	; 0xB8000 | 3 = VGA buffer with present and write/read bits set

	mov eax, PD_OFFSET 		; The address that points to the directory table, 1024 entries, 32 bits each
	mov cr3, eax			; We put the address of our directory table in the cr3 register

	mov eax, cr0			; To enable paging we need to set the correct flags in the cr0 register
	or 	eax, 80000000h
	mov cr0, eax

	jmp genesis 			; Go back to the bootloader to start executing the kernel!
			
			

relocate_kernel:


	; Relocate 16 sectors of kernel from below 1MB to above 1MB
	mov ecx, 800h
	
	mov esi, INIT_KERNEL_OFFSET
	mov edi, PHYS_KERNEL_OFFSET

	rep movsd

	ret

; Map a page table's entries to physical page frames
; In: EDI = Page table offset to fill, ECX = Physical page frame index, EDX = Page directory entry index to point to new table
fill_table:

	pusha
	push edx

	xor ebx, ebx

.fill:
	
	mov eax, 1000h 		; Each page table entry maps 4 kilobytes of data, so we would give the 4k aligned offset each time
	mul ecx 			; Index times 4096 (4k) bytes
	or eax, 3 			; Set present flag to true, as well as read/write

	mov dword [edi], eax	; Map page frame value to entry
	add edi, 4  			; Go to next entry
	inc ecx 
	inc ebx			

	cmp ebx, 1024 	; We only want to map 1024 entries (that make up a page table)
	jl .fill

	sub edi, 1000h 	; Get the initial page table offset

	pop edx

	; Get the physical address of the page directory entry to add table to
	; EDX = Page directory entry index
	mov eax, 4
	mul edx 			; Each page directory entry is 4 bytes
	add eax, PD_OFFSET 	; Add the page directory offset
	mov esi, eax

	mov dword [esi], edi		; Put the page table into the page directory entry 
	or 	dword [esi], 3 			; Turn present flag on

	popa
	ret