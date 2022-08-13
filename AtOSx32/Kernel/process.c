#include "process.h"

aprocess_t* task = NULL; // Current task


void init_multitasking() {

  aprocess_t* current_proc = kmalloc(sizeof(aprocess_t));

  current_proc->state = TASK_ACTIVE;

  current_proc->address_space = cpu_get_address_space(); 
  current_proc->pid = get_next_pid();
  current_proc->esp0 = (uint32_t*)page_map((void*)KERNEL_STACK, 1, READ_WRITE | PRESENT);   // Actually create a stack
  
  current_proc->esp = current_proc->esp0;


  //asm volatile("mov %0, %%esp" : : "r" (current_proc->esp0));
  while(1) {}  
  task = current_proc;

  /*aprocess_t* np = kmalloc(sizeof(aprocess_t));
  np->pid = get_next_pid();
  np->esp0 = kmalloc(0x1000);*/
  

}

uint32_t get_next_pid() {
  static uint32_t next_pid = 8008;
  return next_pid++;
}


