#ifndef MUTEX_H
#define MUTEX_H

#include "process.h"

typedef struct _MUTEX_LOCK_STRUCT {

  task_list_t* wait_list; 
  bool aquired;

} mutex_t;

mutex_t* create_mutex();

void mutex_lock();
void mutex_unlock();

#endif

