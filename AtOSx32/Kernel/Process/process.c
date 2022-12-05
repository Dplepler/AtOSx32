#include "process.h"

uint32_t irq_disable_counter = 0;

tcb_t* running_task   = NULL;  /* Current task */
tcb_t* next_task      = NULL;  /* Buffer of the selected task to run */
tcb_t* scheduler_task = NULL;  /* Default idle task, searches for other tasks to run */
tcb_t* cleaner_task   = NULL;  /* Clean allocated memory of terminated tasks */

task_list_t** available_tasks = NULL;  /* All tasks that are applicable to run */

/* Blocked tasks */
task_list_t* sleeping_tasks   = NULL;  /* Tasks that invoked the sleeping operation or that have ran for too long */
task_list_t* blocked_tasks    = NULL;  /* Tasks that are not allowed to run for any reason */
task_list_t* terminated_tasks = NULL;  /* Tasks that have ended their lifetime */


bool allow_ts = false;  /* Task switching lock */


static inline void lock_ts()   { allow_ts = false; }
static inline void unlock_ts() { allow_ts = true; }

void irq_enable() {

  if (!irq_disable_counter) { return; }
  if (!--irq_disable_counter) { sti(); }
}

void irq_disable() { 
  
  if (!irq_disable_counter++) { cli(); }
} 

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

/* Create a process handler */
process_t* create_process_handler(uint32_t* address_space, uint32_t eip, void* params, uint8_t policy) {
  return (process_t*)create_task_handler(address_space, eip, params, policy);  
}

/* Create a thread handler - address space remains the same as the caller's */
thread_t* create_thread_handler(uint32_t eip, void* params, uint8_t policy){
  return (thread_t*)create_task_handler(running_task->address_space, eip, params, policy);
}

/* Create a mew task handler */
tcb_t* create_task_handler(uint32_t* address_space, uint32_t eip, void* params, uint8_t policy) { 

  irq_disable();
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
  *--stack = (uint32_t)params;
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
  irq_enable();
  
  return new_task;
}

/* Let the cleaner task clear the tasks information */
void terminate_task() {

  /* We can't cleanup the task's stack just yet, we're still in it, so signal to the next task to do so */
  task_block(TASK_TERMINATED);

  /* Schedule the cleaner to free up the process' memory */
  if (cleaner_task->state == TASK_BLOCKED) { task_unblock(cleaner_task); }
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

/* Run the task pointed to by the task buffer */
void run_task() {
  
  if (!next_task) { return; }

  irq_disable();

  next_task->state = TASK_ACTIVE;
  
  switch_task(next_task);
}

void task_change_state(tcb_t* task, uint16_t state) {
  task->state = state;
}

/* Block the currently running task from running */
void task_block(uint32_t new_state) {
 
  lock_ts();
  irq_disable();

  /* Insert task to the blocked list */
  switch (new_state) {
    case TASK_BLOCKED:    task_list_insert_back(blocked_tasks,    running_task); break;
    case TASK_SLEEPING:   task_list_insert_back(sleeping_tasks,   running_task); break;
    case TASK_TERMINATED: task_list_insert_back(terminated_tasks, running_task); break;
    /* Invalid new states */
    case TASK_WAITING: 
    case TASK_ACTIVE: 
    case TASK_AVAILABLE: irq_enable(); return;
  }

  /* Update state */
  task_change_state(running_task, new_state);

  running_task->time_slice = DEFAULT_TIME_SLICE;  
  
  unlock_ts();

  /* Task can't run anymore, find another task */
  schedule();
}

/* Unblock task from any block list, and make it available */
void task_unblock(tcb_t* task) {
 
  if (task == 0xc040303f) { PRINT("HELLO"); }
  irq_disable();
  lock_ts();
 
  /* Remove from blocked list */
  switch (task->state) {
    case TASK_BLOCKED:    task_list_remove_task(blocked_tasks,    task); break;
    case TASK_SLEEPING:   task_list_remove_task(sleeping_tasks,   task); break;
    case TASK_TERMINATED: task_list_remove_task(terminated_tasks, task); break;
    /* Invalid previous states */
    case TASK_WAITING:
    case TASK_ACTIVE: 
    case TASK_AVAILABLE: irq_enable(); return;
  }

  task_change_state(task, TASK_AVAILABLE);

  /* Insert task to the waitlist */
  task_list_insert_back(available_tasks[task->policy], task);
  
  irq_enable();
  unlock_ts();
}

/* Go over all sleeping tasks and reduce their nap time */
void manage_sleeping_tasks() {
    
    irq_disable();
    
    tcb_t* task = sleeping_tasks->head;
 
    while (task) {
      if (time_counter >= task->naptime) { task->naptime = 0; task_unblock(task); }   // Naptime over, task is ready to run
      task = task->flink;
    }

    irq_enable();
}

/* Decrease the time slice for the running program, if the time slice ended, block the task and run another one */
void manage_time_slice() {
  
  irq_disable();
  if (!--running_task->time_slice) { sleep(5); }
  irq_enable();
}

/* Assign the desired time to wake up for a task */
void set_naptime(unsigned long time) {
  running_task->naptime = time;   
}

/* Insert a task to a task list's head */
void task_list_insert_front(task_list_t* list, tcb_t* task) {
  if (!list->head) { list->head = list->tail = task; task->flink = NULL; }
  else { task->flink = list->head; list->head = task; }
}

/* Insert a task to a task list's tail */
void task_list_insert_back(task_list_t* list, tcb_t* task) {
  if (!list->tail) { list->tail = list->head = task; task->flink = NULL; }
  else { list->tail->flink = task; list->tail = task; task->flink = NULL; }
}

/* Abosolutely stunning Linus-inspired code */
void task_list_remove_task(task_list_t* list, tcb_t* task) {
  
  list->tail = list->head;
  tcb_t** indirect = &list->head;

  while ((*indirect) != task) { indirect = (tcb_t**)&((*indirect)->flink); list->tail = *indirect; }

  *indirect = task->flink;
  
  while (list->tail->flink) { list->tail = list->tail->flink; }
  if (!list->head) { list->tail = NULL; }
}

/* Assign an id to a task */
uint32_t get_next_pid() {
  static uint32_t next_pid = 0xB00B;
  return next_pid++;
}

/* Find the next task to run and update the task buffer accordingly */
void schedule() {

  if (!allow_ts) { return; }
  lock_ts();

  /* Get highest policy available task */
  tcb_t* high_policy0_task = schedule_priority_task(available_tasks[POLICY_0]->head);
  tcb_t* high_policy1_task = schedule_priority_task(available_tasks[POLICY_1]->head);

  tcb_t* task = high_policy0_task ? high_policy0_task : high_policy1_task; 

  /* Decrease priority if we got a priority task, if priority is 0 get it back to it's desired level */
  if (task) { if (!--task->priority) { task->priority = task->req_priority; } }
  else { task = schedule_time_slice_task(); }  /* Find a task from lower level policies */

  if (!task) { task = scheduler_task; }  /* Idle mode, keep searching for tasks */
  else { task_list_remove_task(available_tasks[task->policy], task); }  /* Remove task from available task */

  /* Idle mode couldn't find new task, stay idle */
  if (running_task == scheduler_task && task == scheduler_task) { next_task = NULL; unlock_ts(); }
  else { next_task = task; }  /* Update task buffer (run next task) */

  if (next_task) { run_task(); }
}

/* Picks the highest priority task from the task list */
tcb_t* schedule_priority_task(tcb_t* list) {

  tcb_t* highest_priority_task = list;
  tcb_t* task = list;

  /* Get the highest priority task from the list */
  while (task) {
    if (task->priority > highest_priority_task->priority) { highest_priority_task = task; }
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

