#include "tss.h"


/* Install a task state segment gate */
void tss_install(tss_t* task_state, gdtptr* gdtptr, gdt_descriptor** gdt) {

  memset(task_state, 0, sizeof(tss_t));

  task_state->ss0 = GDT_KERNEL_DS;
  task_state->iopb = sizeof(tss_t);
  task_state->esp0 = 1024;
 
  gdt_create_gate(gdt, 5, (uint32_t)task_state, (uint32_t)task_state + sizeof(tss_t) - 1, 0xe9, 0x00);

  cpu_load_gdt(gdtptr);
  cpu_load_tss();
}

