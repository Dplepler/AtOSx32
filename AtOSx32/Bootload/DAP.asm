; This file includes the Disk address packet, we will use this to write the kernel into memory

DAP_START:

	; Size of packet
	.DAP_SIZE:

		db 10h

	; Reserved
	.DAP_NULL:

		db 0

	; Write 50 sectors
	.DAP_SECTORS:

		dw 50

	; Initially we write to a 16 bit offset, in this case 0x1000
	.DAP_KERNEL_OFFSET:

		dw INIT_KERNEL_OFFSET
		dw 0

	; Write third disk sector, where kernel is located on the disk
	.DAP_LBA:

		dd 2
		dd 0

DAP_END:	
