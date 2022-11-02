#include "process.h"

aprocess_t* task = NULL;  // Current task
aprocess_t* root_task = NULL;


void init_multitasking() {

  cli();

  aprocess_t* current_proc = kmalloc(sizeof(aprocess_t));

  current_proc->state = TASK_ACTIVE;
    
  current_proc->address_space = cpu_get_address_space(); 
  current_proc->pid = get_next_pid();
  current_proc->esp0 = INIT_KERNEL_STACK;

  root_task = task = current_proc;
   
  sti();
}

aprocess_t* create_task(uint8_t state, uint32_t* address_space) { 

  cli();
  
  aprocess_t* new_task = kmalloc(sizeof(aprocess_t));
  
  new_task->state = state;
  new_task->address_space = address_space;
  new_task->pid = get_next_pid();
  new_task->esp0 = ((uint32_t)kmalloc(STACK_SIZE)) + KERNEL_STACK - 0x4;   // Create new stack
  
  task->flink = new_task; 

  sti();

  return new_task;
}

void run_task(aprocess_t* new_task) {
  switch_task(new_task);
}

aprocess_t* find_task(uint32_t pid) {

  aprocess_t* curr = root_task;

  while (curr && curr->pid != pid) { curr = curr->flink; }

  return curr;
}

uint32_t get_next_pid() {
  static uint32_t next_pid = 8008;
  return next_pid++;
}

uint32_t* create_address_space() {
  
  uint32_t* address_space = kmalloc(PD_SIZE);

  for (uint32_t i = 0; i < PD_ENTRIES; i++) {
    address_space[i] = READ_WRITE;
  }

  while(1) {}
  return address_space;
}


