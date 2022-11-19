#include "gdt.h"

void setup_gdt(gdtptr* gdt_ptr, gdt_descriptor** gdt) {

  init_gdt(gdt_ptr, gdt);
  cpu_load_gdt(gdt_ptr);
  gdt_install_gates(gdt);
  cpu_load_gdt(gdt_ptr);
}

void init_gdt(gdtptr* gdt_ptr, gdt_descriptor** gdt) {

  gdt_ptr->limit = sizeof(gdt_descriptor) * GDT_SIZE - 1;
  gdt_ptr->offset = (uint32_t)gdt;
}


void gdt_create_gate(gdt_descriptor** gdt, uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
  
  gdt[index] = kcalloc(sizeof(gdt_descriptor));

  gdt[index]->base_l = base & 0xFFFF;
  gdt[index]->base_m = (base >> 16) & 0xFF;
  gdt[index]->base_h = base >> 24;
  gdt[index]->limit  = limit & 0xFFFF;
  gdt[index]->flags_and_limit = limit >> 16;
  gdt[index]->flags_and_limit |= granularity & 0xF0;   // Only the higher half bits
  gdt[index]->access = access;
}

/* Install null gate as well as kernel data and code segments */
void gdt_install_gates(gdt_descriptor** gdt) {

  gdt_create_gate(gdt, 0, 0, 0, 0, 0);                 // First gate is ignored by the CPU
  gdt_create_gate(gdt, 1, 0, 0xFFFFFFFF, 0x9A, 0xCF);  // Kernel code
  gdt_create_gate(gdt, 2, 0, 0xFFFFFFFF, 0x92, 0xCF);  // Kernel data

}

