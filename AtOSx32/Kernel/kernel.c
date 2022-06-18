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

  int* arr = malloc(10);
  PRINTNH(arr);
  NL;

  memset(arr, 'a', 9);
  arr[9] = 0;

  free(arr);
  int* hello = malloc(10);
  PRINTNH(hello);


  return 0;
}