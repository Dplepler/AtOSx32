 /* Check if the compiler thinks you are targeting the wrong operating system */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* Can only compile for 32-bit ix86 targets. */
#if !defined(__i386__)
#error "You need an ix86-elf compiler to build AtOS"
#endif

#include "kernel_screen.h"

int kmain(void) {


	char* ptr = 0;


	/* Initialize terminal interface */
	terminal_initialize();

	char* hello = "hello";

	for (unsigned int i = 0; i < 5; i++) {
		terminal_putchar(hello[i]);
	}


	//terminal_draw_square(0, 0, 20, 10, VGA_COLOR_CYAN);

	//terminal_write("hello world!", 5);
	//terminal_putchar('b');

	return 0;
}