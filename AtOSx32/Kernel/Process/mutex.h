#ifndef MUTEX_H
#define MUTEX_H

#include "process.h"

typedef struct _MUTEX_LOCK_STRUCT {

  tcb_t* waiting_list_head;
  tcb_t* waiting_list_tail;
  
  bool aquired;


} mutex_t;

mutex_t* create_mutex();

void mutex_lock();
void mutex_unlock();

#endif
