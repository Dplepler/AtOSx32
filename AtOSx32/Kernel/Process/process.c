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

aprocess_t* create_process(uint8_t state, uint32_t* address_space) {

  aprocess_t* process = create_task(state, address_space);

  process->address_space;

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
  
  map_higher_half(new_task->address_space);

  PRINTNH(new_task->address_space);
  while(1) {}
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





