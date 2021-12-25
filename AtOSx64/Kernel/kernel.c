 /* Check if the compiler thinks you are targeting the wrong operating system */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* Can only compile for 32-bit ix86 targets. */
#if !defined(__i386__)
#error "You need an ix86-elf compiler to build AtOS"
#endif



int kmain(void) {
	/* Initialize terminal interface */
	terminal_initialize();


	//keyboard_set_caps_led(1);

	keyboard_set_caps_led(1);

	/* Newline support is left as an exercise. */
	

	return 0;
}