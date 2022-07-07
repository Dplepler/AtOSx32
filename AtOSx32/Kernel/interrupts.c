#include "interrupts.h"

InterruptDescriptor idt[IDT_SIZE];
idtptr idt_ptr;

idtptr init_idt() {

  idt_ptr.limit = sizeof(InterruptDescriptor) * 256 - 1;
  memset(idt, 0, sizeof(InterruptDescriptor) * 256);
  idt_ptr.offset = idt;

  return idt_ptr; 
}

void idt_create_gate(uint8_t index, uint32_t address, uint16_t select, uint8_t attributes) {

  InterruptDescriptor entry;
  entry.attributes = attributes;
  entry.offset_lh = (uint16_t)(address & 0xFFFF);
  entry.offset_hh = (uint16_t)(address >> 16);
  entry.selector = select;

  idt[index] = entry;
}
