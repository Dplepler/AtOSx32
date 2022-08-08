#include "tss.h"

tss task_state;

/* Install a task state segment gate */
void tss_install() {

  memset(&task_state, '\0', sizeof(tss));

  task_state.ss0 = GDT_KERNEL_DS;
  task_state.iopb = sizeof(tss);
  task_state.esp0 = 1024;

  gdt_create_gate(5, (uint32_t)&task_state, sizeof(task_state), 0x89, 0x40);
  
  load_gdt();
}

