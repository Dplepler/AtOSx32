#include "process.h"

aprocess* task; // Current task

void init_multitasking() {

  aprocess* current_proc = malloc(sizeof(aprocess));

  asm volatile("mov %%cr3, %0" : "=r" (current_proc->address_space));
  asm volatile("mov %%esp, %0" : "=r" (current_proc->esp));
  
  current_proc->pid = get_next_pid();
  

}

uint32_t get_next_pid() {

  static uint32_t next_pid = 8008;
  return next_pid++;
}
