#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "Memory/heap.h"

#define INIT_KERNEL_STACK 0xC03FE000
#define KERNEL_STACK      0xE0000000
#define STACK_SIZE        0x1000

typedef struct _PROCESS_CONTROL_BLOCK_STRUCT {

  uint32_t eip;
  uint32_t esp0;
  uint32_t esp;
  uint32_t ebp;

  uint32_t* address_space; 
  
  struct _PROCESS_CONTROL_BLOCK_STRUCT* flink;
  
  enum {

    TASK_ACTIVE,
    TASK_AVAILABLE,
    TASK_BLOCKED

  } state;
 
  uint32_t pid;
  uint32_t curr_cpu_time;

} __attribute__((packed)) aprocess_t, athread_t;

extern void switch_task(struct _PROCESS_CONTROL_BLOCK_STRUCT* new_task);

void init_multitasking();
void run_task(aprocess_t* new_task);

uint32_t* create_address_space();
uint32_t* relocate_stack(uint32_t* address, size_t size);
uint32_t get_next_pid();

aprocess_t* create_task(uint8_t state, uint32_t* address_space);
aprocess_t* find_task(uint32_t pid);

#endif
