#include "process.h"

aprocess* task = NULL; // Current task


void init_multitasking() {

  aprocess* current_proc = malloc(sizeof(aprocess));

  asm volatile("mov %%cr3, %0" : "=r" (current_proc->address_space));
  
  current_proc->pid = get_next_pid();
  current_proc->esp0 = page_map(0xe0000000, 1, READ_WRITE | PRESENT);
  
  asm volatile("mov %0, %%esp" : : "r" (current_proc->esp0));
  current_proc->esp = current_proc->esp0;
    
  task = current_proc;
}

uint32_t get_next_pid() {
  static uint32_t next_pid = 8008;
  return next_pid++;
}

