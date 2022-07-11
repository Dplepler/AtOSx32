#include "gdt.h"

static gdtDescriptor gdt[GDT_SIZE];
static gdtptr gdt_ptr;

void setup_gdt() {
  init_gdt();
  load_gdt();
  gdt_install_gates();
}

void init_gdt() {
  gdt_ptr.limit = sizeof(gdtDescriptor) * GDT_SIZE - 1;
  memset(gdt, 0, sizeof(gdtDescriptor) * GDT_SIZE);
  gdt_ptr.offset = (uint32_t)&gdt;
}

void load_gdt() {
  cpu_load_gdt(&gdt_ptr);
}

void gdt_create_gate(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
  gdt[index].base_l = base & 0xFFFF;
  gdt[index].base_m = (base >> 16) & 0xFF;
  gdt[index].base_h = base >> 24;
  gdt[index].limit = limit & 0xFFFF;
  gdt[index].flags_and_limit = limit >> 16;
  gdt[index].flags_and_limit |= granularity & 0xF0;   // Only the higher half bits
  gdt[index].access = access;
}

void gdt_install_gates() {

  gdt_create_gate(0, 0, 0, 0, 0);   // First gate is ignored by the CPU
  gdt_create_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code
  gdt_create_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data

  load_gdt();
}

