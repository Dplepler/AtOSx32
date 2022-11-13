#include "process.h"

tcb_t* running_task = NULL;  // Current task
tcb_t* root_task = NULL;


void init_multitasking() {

  tcb_t* current_proc = kmalloc(sizeof(tcb_t));

  current_proc->state = TASK_ACTIVE;
    
  current_proc->address_space = cpu_get_address_space(); 
  current_proc->pid = get_next_pid();
  current_proc->esp0 = INIT_KERNEL_STACK;

  root_task = running_task = current_proc;   
}

uint32_t* create_address_space() {

  uint32_t* address_space = kmalloc_aligned(PD_SIZE, 0x1000);
   
  /* Reset all entries */
  for (uint32_t i = 0; i < PD_ENTRIES; i++) { address_space[i] |= READ_WRITE; }
  

  /* Map page directory to itself */
  address_space[PD_ENTRIES-1] = (uint32_t)page_physical_address(address_space) | READ_WRITE | PRESENT;

  return address_space;
}


process_t* create_process(uint8_t state, uint32_t* address_space, uint32_t eip) {

  map_higher_half(address_space);
  process_t* process = create_task(state, address_space, eip);

  return process;
}


tcb_t* create_task(uint8_t state, uint32_t* address_space, uint32_t eip) { 
  
  tcb_t* new_task = kmalloc(sizeof(tcb_t));
  
  new_task->state = state;
  new_task->address_space = page_physical_address(address_space);
  new_task->pid = get_next_pid();
  new_task->esp0 = (uint32_t)kmalloc_aligned(STACK_SIZE, 0x1000) + STACK_SIZE - 0x4;   // Create new stack
  new_task->eip = eip;
  
  running_task->flink = new_task; 

  /* Set up initial stack layout to be popped in the task switch routinue */
  new_task->esp = new_task->esp0; 

  return new_task;
}

void terminate_process(tcb_t* task) {

  PRINT("Task: ");
  PRINTN(task->pid);
  PRINT("Terminated\n");
  while(1) {}
}

void run_task(tcb_t* new_task) {

  uint32_t* stack = (uint32_t*)new_task->esp;
  
  *--stack = new_task;
  *--stack = terminate_process;

  if (!new_task->curr_cpu_time) {

    cdecl_regs registers;

    __asm__ __volatile__ ("mov %%ebx, %0" : "=r" (registers.ebx));  
    __asm__ __volatile__ ("mov %%esi, %0" : "=r" (registers.esi));
    __asm__ __volatile__ ("mov %%edi, %0" : "=r" (registers.edi));
    __asm__ __volatile__ ("mov %%ebp, %0" : "=r" (registers.ebp));
    
    *--stack = new_task->eip;
    *--stack = registers.ebx;
    *--stack = registers.esi;
    *--stack = registers.edi;
    *--stack = registers.ebp;
     
    new_task->esp = (uint32_t)stack;
  }

  cli();
  switch_task(new_task);
  sti();
}

tcb_t* find_task(uint32_t pid) {

  tcb_t* curr = root_task;

  while (curr && curr->pid != pid) { curr = curr->flink; }

  return curr;
}

uint32_t get_next_pid() {
  static uint32_t next_pid = 8008;
  return next_pid++;
}

