#include "tss.h"

tss task_state;

/* Install a task state segment gate */
void tss_install() {

  memset(&task_state, '\0', sizeof(tss));

  task_state.ss0 = GDT_KERNEL_DS;
  task_state.iopb = sizeof(tss);
  task_state.esp0 = 1024;
 
  gdt_create_gate(5, &task_state, sizeof(tss) - 1, 0xe9, 0x00);

  cpu_load_tss(&task_state);
  load_gdt();
}


