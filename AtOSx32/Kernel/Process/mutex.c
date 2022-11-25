#include "mutex.h"

uint32_t irq_disable_counter = 0;
extern tcb_t* running_task;


/* Init mutex function */
mutex_t* create_mutex() {
  
  mutex_t* mutex = kmalloc(sizeof(mutex_t));

  mutex->waiting_list_head = mutex->waiting_list_tail = NULL;
    
  mutex->aquired = false;

  return mutex;
}

/* Lock a mutex, if a process/task aquired a mutex, other processes that are trying to aquire it will wait until it is released */
void mutex_lock(mutex_t* mutex) {
  
  if (!mutex->aquired) { mutex->aquired = true; return; }  // Normal lock
  
  running_task->flink = NULL;   // Detach task, we will move it to the waiting task list

  /* If lock is already locked, add task to the waiting list */
  if (!mutex->waiting_list_head) { mutex->waiting_list_head = running_task; }
  else { mutex->waiting_list_tail->flink = running_task; }

  mutex->waiting_list_tail = running_task;
  
  task_block(TASK_WAITING);
}


/* Unlock a mutex, let the first task that has been waiting for that mutex to run, or just mark it as aquirable */
void mutex_unlock(mutex_t* mutex) { 
  
  if (!mutex->waiting_list_head) { mutex->aquired = false; return; }

  task_change_state(mutex->waiting_list_head, TASK_AVAILABLE);
  mutex->waiting_list_head = mutex->waiting_list_head->flink;

}
