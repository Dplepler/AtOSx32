#include "process.h"

uint32_t irq_disable_counter = 0;
bool     allow_ts = false;

tcb_t* running_task = NULL;  // Current task
tcb_t* cleaner_task = NULL;

task_list_t** available_tasks;

task_list_t* sleeping_tasks;
task_list_t* blocked_tasks;
task_list_t* terminated_tasks;



/* The startup task will become our initial process, but also the cleaner task */
void init_multitasking() {

  cleaner_task = kmalloc(sizeof(tcb_t));

  cleaner_task->state = TASK_ACTIVE;   
  cleaner_task->cr3 = (uint32_t)cpu_get_address_space();
  cleaner_task->address_space = NULL;   // Was not malloced
  cleaner_task->pid = get_next_pid();
  cleaner_task->esp0 = INIT_KERNEL_STACK;
  cleaner_task->cpu_time = 0;
  cleaner_task->type     = PROCESS;
  cleaner_task->priority = cleaner_task->req_priority = 0;
  cleaner_task->policy = POLICY_3;            // This is a background task     
   
  running_task = cleaner_task;
  
  /* Initialize all task lists */
  available_tasks = kcalloc(POLICY_AMOUNT, sizeof(task_list_t*));
  for (uint8_t i = 0; i < POLICY_AMOUNT; i++) { available_tasks[i] = kcalloc(1, sizeof(task_list_t)); }
 
  sleeping_tasks   = kcalloc(1, sizeof(task_list_t));
  blocked_tasks    = kcalloc(1, sizeof(task_list_t));
  terminated_tasks = kcalloc(1, sizeof(task_list_t));
  
  unlock_ts();
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


process_t* create_process_handler(uint32_t* address_space, uint32_t eip, void* params, uint8_t policy) {
  return (process_t*)create_task_handler(address_space, eip, params, policy);  
}

thread_t* create_thread_handler(uint32_t eip, void* params, uint8_t policy){
  return (thread_t*)create_task_handler(running_task->address_space, eip, params, policy);
}


tcb_t* create_task_handler(uint32_t* address_space, uint32_t eip, void* params, uint8_t policy) { 

  lock_ts();
  tcb_t* new_task = kmalloc(sizeof(tcb_t));
 
  new_task->state = TASK_AVAILABLE;
  
  new_task->cr3 = (uint32_t)page_physical_address(address_space);
  new_task->address_space = address_space;
  new_task->pid = get_next_pid();
  new_task->esp0 = (uint32_t)kmalloc_aligned(STACK_SIZE, 0x1000) + STACK_SIZE;   // Create new kernel stack
  new_task->eip = eip;
  new_task->cpu_time = 0;
  new_task->esp = new_task->esp0; 
  new_task->policy = policy;

  new_task->time_slice = policy >= POLICY_2 ? DEFAULT_TIME_SLICE : 0;
  new_task->req_priority = new_task->priority = policy <= POLICY_1 ? DEFAULT_PRIORITY   : 0;


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

  task_list_insert_back(available_tasks[new_task->policy], new_task);

  unlock_ts();
  return new_task;
}

 
void init_task(tcb_t* task, void* params) {

  void* (*entry)(void*) = (void*)task->eip; 
  (*entry)(params);
  terminate_task();
}


void terminate_task() {

  /* We can't cleanup the task's stack just yet, we're still in it, so signal to the next task to do so */
  task_block(TASK_TERMINATED);

  /* Schedule the cleaner to free up the process' memory */
  task_unblock(cleaner_task);
}

/* Process to clean up after terminated tasks */
void task_cleaner() {

  tcb_t* task = terminated_tasks->head;

  while (task) {
    task_cleanup(task);
    task = terminated_tasks->head = task->flink;
  }

  task_block(TASK_BLOCKED);
}

/* Free up a task's allocated data */
void task_cleanup(tcb_t* task) {
  free_aligned((void*)(task->esp0 - STACK_SIZE));
  if (task->type == PROCESS && task->address_space) { free(task->address_space); }
  free(task);
} 

void run_task(tcb_t* new_task) {
   
  update_proc_time();
  proc_time_counter = 0;
  
  new_task->state = TASK_ACTIVE;
  new_task->flink = NULL;


  cli();
  switch_task(new_task);
}

void task_change_state(tcb_t* task, uint16_t state) {
  task->state = state;
}

/* Block the currently running task from running */
void task_block(uint32_t new_state) {
  
  switch (new_state) {
    case TASK_BLOCKED:    task_list_insert_back(blocked_tasks,    running_task); break;
    case TASK_SLEEPING:   task_list_insert_back(sleeping_tasks,   running_task); break;
    case TASK_TERMINATED: task_list_insert_back(terminated_tasks, running_task); break;
    case TASK_ACTIVE: 
    case TASK_AVAILABLE: return;
  }

  task_change_state(running_task, new_state);
  schedule();
}


void task_unblock(tcb_t* task) {

  switch (task->state) {
    case TASK_BLOCKED:    task_list_remove_task(blocked_tasks,    task); break;
    case TASK_SLEEPING:   task_list_remove_task(sleeping_tasks,   task); break;
    case TASK_TERMINATED: task_list_remove_task(terminated_tasks, task); break;
    case TASK_ACTIVE: 
    case TASK_AVAILABLE: return;
  }

  task_change_state(task, TASK_AVAILABLE); 

  /* Insert task to the waitlist */
  task_list_insert_back(available_tasks[task->policy], task);
}

/* Go over all sleeping tasks and reduce their nap time */
void manage_sleeping_tasks() {

  tcb_t* task = sleeping_tasks->head;

  while (task) {
    if (task->naptime >= time_counter) { task->naptime = 0; task_unblock(task); }   // Naptime over, task is ready to run
    task = task->flink;
  }
}

void manage_time_slice_tasks() {
  if (!--running_task->time_slice) {
    task_list_insert_back(available_tasks[running_task->policy], running_task);
  } 
}

void insert_sleeping_list(unsigned long time) {

  /* Add running task to sleeping task list */
  task_list_insert_front(sleeping_tasks, running_task);

  running_task->naptime = time; 
}

void task_list_insert_front(task_list_t* list, tcb_t* task) {
  if (!list->head) { list->head = list->tail = task; task->flink = NULL; }
  else { task->flink = list->head; list->head = task; }
}

void task_list_insert_back(task_list_t* list, tcb_t* task) {
  if (!list->tail) { list->tail = list->head = task; task->flink = NULL; }
  else { list->tail->flink = task; list->tail = task; }
}

void task_list_remove_task(task_list_t* list, tcb_t* task) {
  
  task_list_insert_front(list, task);

  tcb_t* it = list->head;

  while (it->flink) {
    if (it->flink == task) {
      it->flink = it->flink->flink;
      break;
    }
  }

  list->head = list->head->flink;
  task->flink = NULL;
}

void update_proc_time() {

  if (!running_task) { idle_time_counter++; return; }    // There doesn't have to be a running process
  
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

  allow_ts = false; 
}

void unlock_ts() {

  allow_ts = true; 
}


void schedule() {
  
  if (!allow_ts) { return; }  // Don't task switch if we are not allowed
  lock_ts();

  tcb_t* high_policy0_task = schedule_priority_task(available_tasks[POLICY_0]->head);
  tcb_t* high_policy1_task = schedule_priority_task(available_tasks[POLICY_1]->head);
 
  tcb_t* task = high_policy0_task ? high_policy0_task : high_policy1_task;
  
  if (task) { if (!--task->priority) { task->priority = task->req_priority; } }
  else { task = schedule_time_slice_task(); }

  if (!task) { return; }  // Give up (idle mode)
  
  /* Remove task from available tasks */
  task_list_remove_task(available_tasks[task->policy], task);       
  run_task(task);
}

/* Picks the highest priority task from the task list */
tcb_t* schedule_priority_task(tcb_t* list) {

  tcb_t* highest_priority_task = list;

  /* Get the highest priority task from the list */
  while (highest_priority_task && highest_priority_task->flink) {
    if (highest_priority_task->flink->priority < highest_priority_task->priority) {
      highest_priority_task = highest_priority_task->flink;
    }
  }
  
  return highest_priority_task;
}

/* Schedule time slice based tasks (policies 2 & 3) */
tcb_t* schedule_time_slice_task() {

  tcb_t* time_slice_task = available_tasks[POLICY_2]->head;
  if (!time_slice_task) { time_slice_task = available_tasks[POLICY_3]->head; }
  
  return time_slice_task;
}




