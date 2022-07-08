/* Check if the compiler thinks you are targeting the wrong operating system */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* Can only compile for 32-bit ix86 targets. */
#if !defined(__i386__)
#error "You need an ix86-elf compiler to build AtOS"
#endif

#include "kernel_keyboard.h"
#include "interrupts.h"
#include "heap.h"

int kmain(void) {
  
  pd_remove_entry(0); 	// Remove identity mapping

  /* Initialize terminal interface */
  terminal_initialize();

  bitmap_mark_kernel();

  perry(25, 5);

  init_idt();
  load_idt();
  idt_install_gates();
  while(1) {}
  int x = 10 / 0;

  
  return 0;
}