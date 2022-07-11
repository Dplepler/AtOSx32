; This file includes the Global Descriptor Table, which is used as a better replacement for real mode segment registers

GDT_START:

	; It is mandatory to add a null descriptor to raise exceptions when needed
	GDT_NULL:
	
		dd 0x0
		dd 0x0
	
	GDT_CODE:
	
		; First Set of flags: 
		;	Present (1 bit)  					on
		;	Priviledge (2 bits) 			off
		; Descriptor Type (1 bit)  	on
		; Code (1 bit) 							on
		; Conforming (1 bit) 				off
		; Readable (1 bit) 					on
		; Accessed (1 bit) 					on
		
		; Second Set of Flags:
		;	Granularity (1 bit) 			on
		; 32 bit default (1 bit)  	on
		; 64 bit segment (1 bit)  	off
		; AVL (1 bit) 							off
			
		dw 0xffff 		; Limit (bits 0 - 15)
		dw 0x0 				; Base (bits 0 - 15)
		db 0x0 				; Base (bits 16 - 23)
		db 10011010b 	; First set of flags
		db 11001111b 	; Second set of flags, limit at bits 0-3
		db 0x0
		
		
	GDT_DATA:
	
		; First Set of flags: 
		;	Present (1 bit)  					on
		;	Priviledge (2 bits) 			off
		; Descriptor Type (1 bit)  	on
		; Code (1 bit) 							off
		; Expend Down (1 bit) 			off
		; Writeable (1 bit) 				on
		; Accessed (1 bit) 					off
		
		; Second Set of Flags:
		;	Granularity (1 bit) 			on
		; 32 bit default (1 bit)  	on
		; 64 bit segment (1 bit)  	off
		; AVL (1 bit) 							off
	
		dw 0xffff 		; Limit (bits 0 - 15)
		dw 0x0 				; Base (bits 0 - 15)
		db 0x0 				; Base (bits 16 - 23)
		db 10010010b 	; First set of flags
		db 11001111b 	; Second set of flags, limit at bits 0-3
		db 0x0 				; Base (bits 24 -31)	
	
	
GDT_END:


; The GDT descriptor 
GDT_DESCRIPTOR:

	dw GDT_END - GDT_START - 1 		; Size of the current GDT (Minus 1)
	dd GDT_START									; Start address of the GDT


; Offsets to quickly access the code and data segments through the segment registers
CODE_SEG equ GDT_CODE - GDT_START
DATA_SEG equ GDT_DATA - GDT_START

