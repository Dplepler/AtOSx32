#include "tss.h"

tss_t task_state;

/* Install a task state segment gate */
void tss_install() {

  memset(&task_state, 0, sizeof(tss_t));

  task_state.ss0 = GDT_KERNEL_DS;
  task_state.iopb = sizeof(tss_t);
  task_state.esp0 = 1024;
 
  gdt_create_gate(5, (uint32_t)&task_state, (uint32_t)&task_state + sizeof(tss_t) - 1, 0xe9, 0x00);

  load_gdt();
  cpu_load_tss();
}
