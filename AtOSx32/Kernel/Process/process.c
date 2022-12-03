#include "process.h"

uint32_t irq_disable_counter = 0;

tcb_t* running_task   = NULL;  // Current task
tcb_t* next_task      = NULL;
tcb_t* scheduler_task = NULL;
tcb_t* cleaner_task   = NULL;

task_list_t** available_tasks;

task_list_t* sleeping_tasks;
task_list_t* blocked_tasks;
task_list_t* terminated_tasks;

bool allow_ts = false;

void lock_ts() { allow_ts = false; }
void unlock_ts() { allow_ts = true; }

/* Initialize all task lists */
void setup_multitasking() {

  available_tasks = kcalloc(POLICY_AMOUNT, sizeof(task_list_t*));
  for (uint8_t i = 0; i < POLICY_AMOUNT; i++) { available_tasks[i] = kcalloc(1, sizeof(task_list_t)); }
 
  sleeping_tasks   = kcalloc(1, sizeof(task_list_t));
  blocked_tasks    = kcalloc(1, sizeof(task_list_t));
  terminated_tasks = kcalloc(1, sizeof(task_list_t));
}

/* The startup task will become our initial process, but also the cleaner task */
void init_multitasking() {

  scheduler_task = kmalloc(sizeof(tcb_t));

  scheduler_task->state = TASK_ACTIVE;   
  scheduler_task->cr3 = (uint32_t)cpu_get_address_space();
  scheduler_task->address_space = NULL;   // Was not malloced
  scheduler_task->pid = get_next_pid();
  scheduler_task->esp0 = INIT_KERNEL_STACK;
  scheduler_task->cpu_time = 0;
  scheduler_task->type     = PROCESS;
  scheduler_task->priority = scheduler_task->req_priority = 127;
  scheduler_task->policy = POLICY_0; 
  scheduler_task->time_slice = DEFAULT_TIME_SLICE;
   
  running_task = scheduler_task;
   
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

  cli();
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

  new_task->time_slice = DEFAULT_TIME_SLICE;
  new_task->req_priority = new_task->priority = policy <= POLICY_1 ? DEFAULT_PRIORITY : 0;


  /* Set up initial stack layout to be popped in the task switch routine */
  uint32_t* stack = (uint32_t*)new_task->esp;  // Temporary stack pointer

  cdecl_regs registers;

  /* Push cdecl registers */
  __asm__ __volatile__ ("mov %%ebx, %0" : "=r" (registers.ebx));  
  __asm__ __volatile__ ("mov %%esi, %0" : "=r" (registers.esi));
  __asm__ __volatile__ ("mov %%edi, %0" : "=r" (registers.edi));
  __asm__ __volatile__ ("mov %%ebp, %0" : "=r" (registers.ebp));
    
  /* Push parameters for make_thread function */ 
  //*--stack = (uint32_t)params;
  *--stack = (uint32_t)new_task->eip;

  /* Push cdecl registers */
  *--stack = registers.ebx;
  *--stack = registers.esi;
  *--stack = registers.edi;
  *--stack = registers.ebp;

  /* Update stack */
  new_task->esp = (uint32_t)stack;


  task_list_insert_back(available_tasks[new_task->policy], new_task);
 
  unlock_ts();
  sti();
  
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

 
void run_task() {
  
  if (!next_task) { return; }

  cli();
 
  next_task->state = TASK_ACTIVE;
  next_task->flink = NULL;
  
  switch_task(next_task);
}

void task_change_state(tcb_t* task, uint16_t state) {
  task->state = state;
}

/* Block the currently running task from running */
void task_block(uint32_t new_state) {
 
  lock_ts();
  cli();

  switch (new_state) {
    case TASK_BLOCKED:    task_list_insert_back(blocked_tasks,    running_task); break;
    case TASK_SLEEPING:   task_list_insert_back(sleeping_tasks,   running_task); break;
    case TASK_TERMINATED: task_list_insert_back(terminated_tasks, running_task); break;
    /* Invalid new states */
    case TASK_WAITING: 
    case TASK_ACTIVE: 
    case TASK_AVAILABLE: sti(); return;
  }
  
  task_change_state(running_task, new_state);
  
  unlock_ts();
  sti();
  schedule();
}


void task_unblock(tcb_t* task) {
 
  cli();
  lock_ts();
 
  switch (task->state) {
    case TASK_BLOCKED:    task_list_remove_task(blocked_tasks,    task); break;
    case TASK_SLEEPING:   task_list_remove_task(sleeping_tasks,   task); break;
    case TASK_TERMINATED: task_list_remove_task(terminated_tasks, task); break;
    /* Invalid previous states */
    case TASK_WAITING:
    case TASK_ACTIVE: 
    case TASK_AVAILABLE: sti(); return;
  }

  task_change_state(task, TASK_AVAILABLE);
 
  /* Insert task to the waitlist */
  task_list_insert_back(available_tasks[task->policy], task);
  
  sti();
  unlock_ts();
}

/* Go over all sleeping tasks and reduce their nap time */
void manage_sleeping_tasks() {
    
    cli();

    tcb_t* task = sleeping_tasks->head;
   
    while (task) {
      if (time_counter >= task->naptime) { task->naptime = 0; task_unblock(task); }   // Naptime over, task is ready to run
      task = task->flink;
    }

    sti();
}

void manage_time_slice() {
  
  cli();
  if (!--running_task->time_slice) {
    running_task->time_slice = DEFAULT_TIME_SLICE;
    sleep(DEFAULT_TIME_SLICE);
  }
  sti();
}

void set_naptime(unsigned long time) {
  running_task->naptime = time; 
}

void task_list_insert_front(task_list_t* list, tcb_t* task) {
  if (!list->head) { list->head = list->tail = task; task->flink = NULL; }
  else { task->flink = list->head; list->head = task; }
}

void task_list_insert_back(task_list_t* list, tcb_t* task) {
  if (!list->tail) { list->tail = list->head = task; task->flink = NULL; }
  else { list->tail->flink = task; list->tail = task; task->flink = NULL; }
}

/* Abosolutely stunning Linus-inspired code */
void task_list_remove_task(task_list_t* list, tcb_t* task) {
  
  list->tail = list->head;
  tcb_t** indirect = &list->head;

  while ((*indirect) != task) { indirect = &((*indirect)->flink); list->tail = *indirect; }

  *indirect = task->flink;
  
  while (list->tail->flink) { list->tail = list->tail->flink; }
  if (!list->head) { list->tail = NULL; }
  
}


uint32_t get_next_pid() {
  static uint32_t next_pid = 8008;
  return next_pid++;
}


void schedule() {

  if (!allow_ts) { return; }  // Don't task switch if it is forbidden
  //cli(); 
  lock_ts();
  
  tcb_t* high_policy0_task = schedule_priority_task(available_tasks[POLICY_0]->head);
  tcb_t* high_policy1_task = schedule_priority_task(available_tasks[POLICY_1]->head);

  tcb_t* task = high_policy0_task ? high_policy0_task : high_policy1_task;

  if (task) { if (!--task->priority) { task->priority = task->req_priority; } }
  else { task = schedule_time_slice_task(); }

  if (!task) { task = scheduler_task; }  /* Idle mode, keep searching for tasks */
  else { task_list_remove_task(available_tasks[task->policy], task); }  /* Remove task from available tasks */

  next_task = task;

  if (running_task == scheduler_task && task == scheduler_task) { 
    next_task = NULL;
    unlock_ts();
  }

 // PRINTNH(next_task); NL;
  next_task = (running_task == scheduler_task && task == scheduler_task) ? NULL : task;
 

  //sti();
}

/* Picks the highest priority task from the task list */
tcb_t* schedule_priority_task(tcb_t* list) {

  tcb_t* highest_priority_task = list;
  tcb_t* task = list;

  /* Get the highest priority task from the list */
  while (task) {
    if (task->priority > highest_priority_task->priority) {
      highest_priority_task = task;
    }
    task = task->flink;
  }
  
  return highest_priority_task;
}

/* Schedule time slice based tasks (policies 2 & 3) */
tcb_t* schedule_time_slice_task() {

  tcb_t* time_slice_task = available_tasks[POLICY_2]->head;
  if (!time_slice_task) { time_slice_task = available_tasks[POLICY_3]->head; }
  
  return time_slice_task;
}




