#include "process.h"

uint32_t irq_disable_counter = 0;
bool     allow_ts = true;

tcb_t* running_task = NULL;  // Current task

task_list_t available_tasks[] = { { NULL, NULL }, { NULL, NULL }, { NULL, NULL }, { NULL, NULL } };
task_list_t waiting_tasks[]   = { { NULL, NULL }, { NULL, NULL }, { NULL, NULL }, { NULL, NULL } };

tcb_t* sleeping_tasks_head = NULL;
tcb_t* terminated_tasks_head = NULL;


void init_multitasking() {

  tcb_t* current_proc = kmalloc(sizeof(tcb_t));

  current_proc->state = TASK_ACTIVE;
    
  current_proc->cr3 = (uint32_t)cpu_get_address_space();
  current_proc->address_space = NULL;   // Was not malloced
  current_proc->pid = get_next_pid();
  current_proc->esp0 = INIT_KERNEL_STACK;
  current_proc->cpu_time = 0;
  current_proc->type     = PROCESS;
  current_proc->priority = 255;
  current_proc->policy = POLICY_3;     
  current_proc->state = TASK_TERMINATED; // We want to terminate this process as soon as possible

  running_task = current_proc; 
}

/* Creates a virtual address space */
uint32_t* create_address_space() {

  uint32_t* address_space = kmalloc_aligned(PD_SIZE, 0x1000);
  
  /* Reset all entries */
  for (uint32_t i = 0; i < PD_ENTRIES; i++) { address_space[i] |= READ_WRITE; }
  
  /* Map page directory to itself */
  address_space[PD_ENTRIES-1] = (uint32_t)page_physical_address(address_space) | READ_WRITE | PRESENT;

  map_higher_half(address_space);
  return address_space;
}


process_t* create_process_handler(uint8_t state, uint32_t* address_space, uint32_t eip, void* params, uint8_t policy) {
  return (process_t*)create_task_handler(state, address_space, eip, params, policy, PROCESS);  
}

thread_t* create_thread_handler(uint8_t state, uint32_t eip, void* params, uint8_t policy){
  return (thread_t*)create_task_handler(state, running_task->address_space, eip, params, policy, THREAD);
}


tcb_t* create_task_handler(uint8_t state, uint32_t* address_space, uint32_t eip, void* params, uint8_t policy, uint8_t type) { 
  
  tcb_t* new_task = kmalloc(sizeof(tcb_t));
  
  new_task->state = state;
  new_task->cr3 = (uint32_t)page_physical_address(address_space);
  new_task->address_space = address_space;
  new_task->pid = get_next_pid();
  new_task->esp0 = (uint32_t)kmalloc_aligned(STACK_SIZE, 0x1000) + STACK_SIZE;   // Create new kernel stack
  new_task->eip = eip;
  new_task->cpu_time = 0;
  
  running_task->flink = new_task; 

  new_task->esp = new_task->esp0; 

  new_task->type = type;
 
  new_task->policy = policy;

  new_task->time_slice = policy >= POLICY_2 ? DEFAULT_TIME_SLICE : 0;
  new_task->priority   = policy <= POLICY_1 ? DEFAULT_PRIORITY   : 0;

  /* Set up initial stack layout to be popped in the task switch routine */
  uint32_t* stack = (uint32_t*)new_task->esp;  // Temporary stack pointer

  cdecl_regs registers;

  /* Push cdecl registers */
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

  return new_task;
}

 
void init_task(tcb_t* task, void* params) {
  
  void* (*entry)(void*) = (void*)task->eip;  
  (*entry)(params);
  terminate_task(task);
}


void terminate_task(tcb_t* task) {

  /* We can't cleanup the task's stack just yet, we're still in it, so signal to the next task to do so */
  task_change_state(task, TASK_TERMINATED);
  task->flink = terminated_tasks_head;
  terminated_tasks_head = task;

  // TODO: Cleaner task should be unblocked here to signal that there is a task that needs to be cleared
  //task_unblock(cleaner);

}

void task_cleaner() {

  tcb_t* task = terminated_tasks_head;

  while (task) {
    task_cleanup(task);
    task = terminated_tasks_head = task->flink;
  }
}

void task_cleanup(tcb_t* task) {
  free_aligned((void*)(task->esp0 - STACK_SIZE));
  if (task->type == PROCESS && task->address_space) { free(task->address_space); }
  free(task);
} 

void run_task(tcb_t* new_task) {
  
  if (!allow_ts) { return; }  // Don't task switch if we are not allowed
                              
  update_proc_time();
  proc_time_counter = 0;
  
  cli();
  new_task->state = TASK_ACTIVE;
  switch_task(new_task);
}

void task_change_state(tcb_t* task, uint16_t state) {
  task->state = state;
}

void task_block(uint32_t new_state) {
  
  task_change_state(running_task, new_state);
  schedule();
}


void task_unblock(tcb_t* task) {

  task_change_state(task, TASK_AVAILABLE); 

  task->flink = NULL;
  
  /* If there are no available tasks we can switch to this one */
  if (!available_tasks[task->policy].head) { run_task(task); } 
  else {
    /* Insert task to the waitlist */
    available_tasks[task->policy].tail->flink = task;
    available_tasks[task->policy].tail = task;
  }
}

/* Go over all sleeping tasks and reduce their nap time */
void manage_sleeping_tasks() {

  tcb_t* task = sleeping_tasks_head;

  while (task) {
    if (!--task->naptime) { task_unblock(task); }   // Naptime over, task is ready to run
    task = task->flink;
  }
}

void insert_sleeping_list(unsigned long hertz) {

  /* Add running task to sleeping task list */
  if (!sleeping_tasks_head) { sleeping_tasks_head = running_task; running_task->flink = NULL; }
  else { running_task->flink = sleeping_tasks_head; sleeping_tasks_head = running_task; }

  running_task->naptime = hertz; 
}

void update_proc_time() {

  if (!running_task) { return; }
  
  /* Prevent a hack that will initialize a process twice if it ran for less than a milisecond */
  running_task->cpu_time += proc_time_counter ? proc_time_counter : 1;
}

/*tcb_t* find_task(uint32_t pid) {

  tcb_t* curr = root_task;

  while (curr && curr->pid != pid) { curr = curr->flink; }

  return curr;
} */

uint32_t get_next_pid() {
  static uint32_t next_pid = 8008;
  return next_pid++;
}

void lock_ts() {

  if (!irq_disable_counter++) { cli(); allow_ts = false; } 
}

void unlock_ts() {

  if (!--irq_disable_counter) { sti(); allow_ts = true; }
}


void schedule() {
  



}

void schedule_high_priority() {


}

void schedule_low_priority() {


}



