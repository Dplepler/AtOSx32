/* Check if the compiler thinks you are targeting the wrong operating system */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* Can only compile for 32-bit ix86 targets. */
#if !defined(__i386__)
#error "You need an ix86-elf compiler to build AtOS"
#endif

#include "kernel_keyboard.h"
#include "paging.h"

int kmain(void) {
  

  pd_remove_entry(0); 	// Remove identity mapping

  /* Initialize terminal interface */
  terminal_initialize();

  terminal_draw_square(30, 0, 20, 10, VGA_COLOR_CYAN);

  bitmap_mark_kernel();

  page_map(0x1000, 0x1000, 2);

  return 0;
}