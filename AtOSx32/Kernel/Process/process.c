#include "process.h"

tcb_t* running_task = NULL;  // Current task
tcb_t* root_task = NULL;


void init_multitasking() {

  tcb_t* current_proc = kmalloc(sizeof(tcb_t));

  current_proc->state = TASK_ACTIVE;
    
  current_proc->cr3 = cpu_get_address_space(); 
  current_proc->pid = get_next_pid();
  current_proc->esp0 = INIT_KERNEL_STACK;
  current_proc->cpu_time = 0;

  root_task = running_task = current_proc;   
}

/* Creates a virtual address space, note that only this function can modify the address space from within the caller's virtual address space */
uint32_t* create_address_space() {

  uint32_t* address_space = kmalloc_aligned(PD_SIZE, 0x1000);
  
  /* Reset all entries */
  for (uint32_t i = 0; i < PD_ENTRIES; i++) { address_space[i] |= READ_WRITE; }
  
  /* Map page directory to itself */
  address_space[PD_ENTRIES-1] = (uint32_t)page_physical_address(address_space) | READ_WRITE | PRESENT;

  map_higher_half(address_space);
  return page_physical_address(address_space);
}


process_t* create_process_handler(uint8_t state, uint32_t* address_space, uint32_t eip) {
  return (process_t*)create_task_handler(state, address_space, eip);  
}

thread_t* create_thread_handler(uint8_t state, uint32_t eip){
  return (thread_t*)create_task_handler(state, running_task->cr3, eip);
}


tcb_t* create_task_handler(uint8_t state, uint32_t cr3, uint32_t eip) { 
  
  tcb_t* new_task = kmalloc(sizeof(tcb_t));
  
  new_task->state = state;
  new_task->cr3   = cr3;
  new_task->pid   = get_next_pid();
  new_task->esp0  = (uint32_t)kmalloc_aligned(STACK_SIZE, 0x1000) + STACK_SIZE;   // Create new kernel stack
  new_task->eip   = eip;
  new_task->cpu_time = 0;
  
  running_task->flink = new_task; 

  /* Set up initial stack layout to be popped in the task switch routinue */
  new_task->esp = new_task->esp0; 

  return new_task;
}

 
void make_thread(tcb_t* task, void* params) {
  
  void* (*entry)(void*) = (void*)task->eip;  
  (*entry)(params);
  terminate_process(task);
}


void terminate_process(tcb_t* task) {

  PRINT("Task: ");
  PRINTN(task->pid);
  PRINT(" Terminated\n\r");

  while(1) {}
}


void run_task(tcb_t* new_task, void* params) {

  uint32_t* stack = (uint32_t*)new_task->esp;

  if (!new_task->cpu_time) {
    
    cdecl_regs registers;

    __asm__ __volatile__ ("mov %%ebx, %0" : "=r" (registers.ebx));  
    __asm__ __volatile__ ("mov %%esi, %0" : "=r" (registers.esi));
    __asm__ __volatile__ ("mov %%edi, %0" : "=r" (registers.edi));
    __asm__ __volatile__ ("mov %%ebp, %0" : "=r" (registers.ebp));
    
    /* Push parameters for make_thread function */ 
    *--stack = (uint32_t)params;
    *--stack = (uint32_t)new_task;

    /* Push cdecl registers */
    *--stack = registers.ebx;
    *--stack = registers.esi;
    *--stack = registers.edi;
    *--stack = registers.ebp;
     
    /* Update stack */
    new_task->esp = (uint32_t)stack;

  }

  update_proc_time();
  proc_time_counter = 0;
  
  cli();
  switch_task(new_task);
}

void update_proc_time() {

  if (!running_task) { return; }
  
  /* Prevent a hack that will initialize a process twice if it ran for less than a milisecond */
  running_task->cpu_time += proc_time_counter ? proc_time_counter : 1;
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

