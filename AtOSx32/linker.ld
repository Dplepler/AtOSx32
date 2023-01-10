OUTPUT_FORMAT("elf32-i386")
ENTRY (kmain)

SECTIONS
{
  . = 0x100000;
	/* The kernel will live at 3GB in the virtual address space, */
	/* which will be mapped to 1MB in the physical address space. */

	. = 0xC0001000;

  /* Add a symbol that indicates the start address of the kernel. */
	_kernel_start = .;

	
	.text ALIGN (4K) : AT (ADDR (.text) - _kernel_start)
	{
		*(.text)
    *(.rodata*)
	}
	.data ALIGN (4K) : AT (ADDR (.data) - _kernel_start)
	{
		*(.data)
	}
	.bss ALIGN (4K) : AT (ADDR (.bss) - _kernel_start)
	{
		*(COMMON)
		*(.bss)
	}
	/* Indicates the end address of the kernel. */
	_kernel_end = .; 
}

