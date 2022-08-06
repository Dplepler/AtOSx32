#include "gdt.h"

static gdt_descriptor gdt[GDT_SIZE];
static gdtptr gdt_ptr;

tss task_state;

void setup_gdt() {
  init_gdt();
  load_gdt();
  gdt_install_gates();
}

void init_gdt() {
  gdt_ptr.limit = sizeof(gdt_descriptor) * GDT_SIZE - 1;
  memset(gdt, 0, sizeof(gdt_descriptor) * GDT_SIZE);
  gdt_ptr.offset = (uint32_t)&gdt;
}

void load_gdt() {
  cpu_load_gdt(&gdt_ptr);
}

void gdt_create_gate(uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {

  static uint32_t index = 0;
  
  gdt[index].base_l = base & 0xFFFF;
  gdt[index].base_m = (base >> 16) & 0xFF;
  gdt[index].base_h = base >> 24;
  gdt[index].limit = limit & 0xFFFF;
  gdt[index].flags_and_limit = limit >> 16;
  gdt[index].flags_and_limit |= granularity & 0xF0;   // Only the higher half bits
  gdt[index++].access = access;

}

/* Install null gate as well as kernel data and code segments */
void gdt_install_gates() {

  gdt_create_gate(0, 0, 0, 0);   // First gate is ignored by the CPU
  gdt_create_gate(0, 0xFFFFFFFF, 0x9A, 0xCF); // Code
  gdt_create_gate(0, 0xFFFFFFFF, 0x92, 0xCF); // Data

  load_gdt();
}

/* Install a task state segment gate */
void gdt_install_tss() {

  task_state.ss0 = GDT_KERNEL_DS;
  task_state.esp0 = 1024;
  task_state.iopb = sizeof(tss);
  gdt_create_gate((uint32_t)&task_state, sizeof(task_state), 0x89, 0x40);
  load_gdt();
}
