/* Check if the compiler thinks you are targeting the wrong operating system */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* Can only compile for 32-bit ix86 targets. */
#if !defined(__i386__)
#error "You need an ix86-elf compiler to build AtOS"
#endif

#include "kernel_keyboard.h"
#include "heap.h"

int kmain(void) {
  
  pd_remove_entry(0); 	// Remove identity mapping

  /* Initialize terminal interface */
  terminal_initialize();

  bitmap_mark_kernel();

  perry(25, 5);

  char* str = malloc(5);
  terminal_write_int(str, 16);
  PRINT_NEW_LINE;
  // char* mmm = malloc(10);
  // terminal_write_int(mmm, 16);
  // PRINT_NEW_LINE;
  // char* aaa = malloc(20);
  // terminal_write_int(aaa, 16);

  return 0;
}