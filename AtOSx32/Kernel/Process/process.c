#include "process.h"

aprocess_t* task = NULL; // Current task


void init_multitasking() {

  aprocess_t* current_proc = kmalloc(sizeof(aprocess_t));

  current_proc->state = TASK_ACTIVE;

  current_proc->address_space = cpu_get_address_space(); 
  current_proc->pid = get_next_pid();
  current_proc->esp0 = (uint32_t*)page_map((void*)KERNEL_STACK, 1, READ_WRITE | PRESENT) + STACK_SIZE - 1;   // Actually create a stack
                                                                                                                                                                                  
  uint32_t prev_esp; asm volatile("mov %%esp, %0" : "=r" (prev_esp));
  
  current_proc->esp = current_proc->esp0 - (prev_esp - (uint32_t)page_physical_address((void*)INIT_KERNEL_STACK));

  for (int i = 0; i < 100; i++) { asm("push $0x69"); }

  memcpy(current_proc->esp0, (uint8_t*)INIT_KERNEL_STACK, STACK_SIZE);   // Relocate kernel stack
   
  asm volatile("mov %0, %%esp" : : "r" (page_physical_address(current_proc->esp)));
  while(1) {}
  task = current_proc;
  
  while(1) {}
  asm volatile("push $0x11");
  

  /*aprocess_t* np = kmalloc(sizeof(aprocess_t));
  np->pid = get_next_pid();
  np->esp0 = kmalloc(0x1000);*/
  

}

uint32_t get_next_pid() {
  static uint32_t next_pid = 8008;
  return next_pid++;
}


