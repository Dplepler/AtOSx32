#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "Memory/heap.h"

#define INIT_KERNEL_STACK 0xC03FE000
#define STACK_SIZE        0x1000

extern unsigned long proc_time_counter; 

typedef struct _TASK_CONTROL_BLOCK_STRUCT {

  uint32_t eip;
  uint32_t esp0;
  uint32_t esp;
  uint32_t ebp;

  uint32_t cr3;
  
  struct _TASK_CONTROL_BLOCK_STRUCT* flink;

  uint32_t pid;
  uint32_t cpu_time;


  enum {

    TASK_ACTIVE,
    TASK_AVAILABLE,
    TASK_BLOCKED,
    TASK_SLEEPING,
    TASK_WAITING

  } state;
 

} __attribute__((packed)) tcb_t, process_t, thread_t;

typedef struct _CDECL_REGISTERS_STRUCT {

  uint32_t ebp;
  uint32_t edi;
  uint32_t esi;
  uint32_t ebx;

} cdecl_regs;


extern void switch_task(struct _TASK_CONTROL_BLOCK_STRUCT* new_task);

void init_multitasking();
void run_task(tcb_t* new_task, void* params);
void terminate_task(tcb_t* task);
void init_task(tcb_t* task, void* params);
void update_proc_time();
void task_change_state(tcb_t* task, uint16_t state);
void task_block(uint32_t new_state);

  uint32_t* create_address_space();
uint32_t* relocate_stack(uint32_t* address, size_t size);
uint32_t get_next_pid();

tcb_t* create_task_handler(uint8_t state, uint32_t cr3, uint32_t eip);
process_t* create_process_handler(uint8_t state, uint32_t* address_space, uint32_t eip);
thread_t* create_thread_handler(uint8_t state, uint32_t eip);
tcb_t* find_task(uint32_t pid);

#endif
