#include "mutex.h"

extern tcb_t* running_task;


/* Init mutex function */
mutex_t* create_mutex() {
  
  mutex_t* mutex = kmalloc(sizeof(mutex_t));
  mutex->wait_list = kmalloc(sizeof(task_list_t));
  mutex->wait_list->head = mutex->wait_list->tail = NULL; 
  mutex->aquired = false;

  return mutex;
}

/* Lock a mutex, if a process/task aquired a mutex, other processes that are trying to aquire it will wait until it is released */
void mutex_lock(mutex_t* mutex) {
  
  if (!mutex->aquired) { mutex->aquired = true; return; }  // Normal lock
  
  running_task->flink = NULL;   // Detach task, we will move it to the waiting task list

  /* If lock is already locked, add task to the waiting list */
  if (!mutex->wait_list->head) { mutex->wait_list->head = running_task; }
  else { mutex->wait_list->tail->flink = running_task; }

  mutex->wait_list->tail = running_task;
  
  task_block(TASK_WAITING);
}


/* Unlock a mutex, let the first task that has been waiting for that mutex to run, or just mark it as aquirable */
void mutex_unlock(mutex_t* mutex) { 
  
  if (!mutex->wait_list->head) { mutex->aquired = false; return; } 
  
  task_unblock(mutex->wait_list->head);
  mutex->wait_list->head = mutex->wait_list->head->flink;
}

