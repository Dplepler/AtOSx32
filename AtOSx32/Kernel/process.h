#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "heap.h"

#define INIT_KERNEL_STACK 0x90000
#define VIRTUAL_SPACE     0x1000000


typedef struct _PROCESS_CONTROL_BLOCK_STRUCT {

  uint32_t* eip;
  uint32_t* esp0;
  uint32_t* esp;

  uint32_t* address_space; 
  
  struct _PROCESS_CONTROL_BLOCK_STRUCT* task_flink;
  
  enum  {

    TASK_ACTIVE,

  } state;
 
  uint32_t pid;
  uint32_t curr_cpu_time;

} __attribute__((packed)) aprocess, athread;



void init_multitasking();
uint32_t* relocate_stack(uint32_t* address, size_t size);

uint32_t get_next_pid();

#endif
