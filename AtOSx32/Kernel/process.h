#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "heap.h"

#define INIT_VIRTUAL_ADDRESS 0x1000000

typedef struct _PROCESS_CONTROL_BLOCK_STRUCT {

  uint32_t esp0;
  uint32_t esp;
  uint32_t address_space;
  
  struct _PROCESS_CONTROL_BLOCK_STRUCT* task_flink;
  
  enum  {

    TASK_ACTIVE,

  } state;
 
  uint32_t pid;
  uint32_t curr_cpu_time;

} proc_control, thread_control;

void init_multitasking();
uint32_t get_next_pid();



#endif
