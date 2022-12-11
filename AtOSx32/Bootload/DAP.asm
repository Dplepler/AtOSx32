; This file includes the Disk Address Packet, we will use this to write the kernel into memory

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

	; Load from disk, 101h's sector (after FAT and boot)
	.DAP_LBA:

		dd 101h
		dd 0

DAP_END:	
