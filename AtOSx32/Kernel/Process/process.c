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

uint32_t* create_address_space() {

  uint32_t* address_space = kmalloc_aligned(PD_SIZE, 0x1000);
   
  /* Reset all entries */
  for (uint32_t i = 0; i < PD_ENTRIES; i++) { address_space[i] |= READ_WRITE | PRESENT; }

  /* Map page directory to itself */
  address_space[PD_ENTRIES-1] = (uint32_t)page_physical_address(address_space) | READ_WRITE | PRESENT;

  return address_space;
}


aprocess_t* create_process(uint8_t state, uint32_t* address_space) {

  map_higher_half(address_space);
  aprocess_t* process = create_task(state, address_space);

  return process;
}


aprocess_t* create_task(uint8_t state, uint32_t* address_space) { 

  cli();
  
  aprocess_t* new_task = kmalloc(sizeof(aprocess_t));
  
  new_task->state = state;
  new_task->address_space = page_physical_address(address_space);
  new_task->pid = get_next_pid();
  new_task->esp0 = (uint32_t)kmalloc_aligned(STACK_SIZE, 0x1000) + STACK_SIZE - 0x4;   // Create new stack
  new_task->esp = new_task->esp0 - 0x4;

  task->flink = new_task; 

  sti();

  return new_task;
}


void run_task(aprocess_t* new_task) {

  
  PRINTNH(new_task->address_space);
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

