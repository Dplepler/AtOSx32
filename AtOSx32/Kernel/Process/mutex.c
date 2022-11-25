#include "mutex.h"

uint32_t irq_disable_counter = 0;

mutex_t* create_mutex() {
  
  mutex_t* mutex = kmalloc(sizeof(mutex_t));

  mutex->waiting_list_head = mutex->waiting_list_tail = NULL;
    
  mutex->aquired = false;

  return mutex;
}


void mutex_lock(mutex_t* mutex) {
  
  if (!mutex->aquired) { mutex->aquired = true; return; }  // Normal lock
  
  running_task->flink = running_task->blink = NULL;   // Detach task, we will move it to the waiting task list

  /* If lock is already locked, add task to the waiting list */
  if (!mutex->waiting_list_head) { mutex->waiting_list_head = running_task; }
  else { 
    mutex->waiting_list_tail->flink = running_task; 
    running_task->blink = mutex->waiting_list_tail;
  }

  mutex->waiting_list_tail = running_task;

  // Block task here

}


void mutex_unlock() {


}
