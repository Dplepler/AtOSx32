OUTPUT_FORMAT("binary")
ENTRY (kmain)

SECTIONS
{
  . = 0x100000;
	/* The kernel will live at 3GB in the virtual address space, */
	/* which will be mapped to 1MB in the physical address space. */

	. = 0xC0000000;

	_kernel_start = .;

	/* Add a symbol that indicates the start address of the kernel. */
	.text ALIGN (4K) : AT (ADDR (.text) - _kernel_start)
	{
		*(.text)
	}
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - _kernel_start)
	{
		*(.rodata)
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