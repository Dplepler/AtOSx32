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

  idt[index].attributes = attributes;
  idt[index].offset_lh  = (uint16_t)(address & 0xFFFF);
  idt[index].offset_hh  = (uint16_t)(address >> 16);
  idt[index].selector   = select;
}

