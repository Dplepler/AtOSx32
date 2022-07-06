#include "interrupts.h"

InterruptDescriptor idt[IDT_SIZE];
idtptr idt_ptr;

void init_idt() {
  idt_ptr.limit = sizeof(InterruptDescriptor) * 256 - 1;
  memset(idt, 0, sizeof(InterruptDescriptor) * 256);
  idt_ptr.offset = idt;
}