#include "process.h"

aprocess_t* task = NULL; // Current task


void init_multitasking() {

  cli();

  aprocess_t* current_proc = kmalloc(sizeof(aprocess_t));

  current_proc->state = TASK_ACTIVE;

  current_proc->address_space = cpu_get_address_space(); 
  current_proc->pid = get_next_pid();
  current_proc->esp0 = INIT_KERNEL_STACK;

  task = current_proc;
 
  sti();
}

aprocess_t* create_task(uint8_t state, uint32_t* address_space) { 

  cli();
  
  aprocess_t* task = kmalloc(sizeof(aprocess_t));
  
  task->state = state;
  task->address_space = address_space;
  task->pid = get_next_pid();
  task->esp0 = ((uint32_t)kmalloc(STACK_SIZE)) + KERNEL_STACK - 0x4;   // Create new stack
  

  sti();

  return task;
}


uint32_t get_next_pid() {
  static uint32_t next_pid = 8008;
  return next_pid++;
}


