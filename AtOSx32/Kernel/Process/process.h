#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "Memory/heap.h"

#define INIT_KERNEL_STACK 0xC03FE000
#define STACK_SIZE        0x1000

#define DEFAULT_TIME_SLICE 50
#define DEFAULT_PRIORITY   128


extern unsigned long proc_time_counter; 
extern unsigned long time_counter;
extern unsigned long idle_time_counter;

typedef struct _TASK_CONTROL_BLOCK_STRUCT {

  uint32_t eip;
  uint32_t esp0;
  uint32_t esp;
  uint32_t ebp;

  uint32_t cr3;
  
  struct _TASK_CONTROL_BLOCK_STRUCT* flink;

  uint32_t pid;
  uint32_t cpu_time;

  uint32_t* address_space;

  enum {
    PROCESS,
    THREAD
  } type;

  enum {

    TASK_ACTIVE,
    TASK_AVAILABLE,
    TASK_TERMINATED,
    TASK_BLOCKED,
    TASK_SLEEPING,
    TASK_WAITING

  } state;

  /* Priority of process */
  enum {

    POLICY_0,
    POLICY_1,
    POLICY_2,
    POLICY_3

  } policy;

  uint32_t time_slice;   // Only for policies 2 & 3
  uint8_t priority;      // Only for policies 0 & 1
  
  uint32_t naptime;      // Duration of sleep
                         
                    

} __attribute__((packed)) tcb_t, process_t, thread_t;

typedef struct _WAITING_LIST_STRUCT {

  tcb_t* tail;
  tcb_t* head;

} task_list_t;



typedef struct _CDECL_REGISTERS_STRUCT {

  uint32_t ebp;
  uint32_t edi;
  uint32_t esi;
  uint32_t ebx;

} cdecl_regs;


extern void switch_task(struct _TASK_CONTROL_BLOCK_STRUCT* new_task);

void init_multitasking();
void run_task(tcb_t* new_task);
void terminate_task();
void init_task(tcb_t* task, void* params);
void update_proc_time();
void task_change_state(tcb_t* task, uint16_t state);
void task_block(uint32_t new_state);
void lock_ts();
void unlock_ts();
void task_unblock(tcb_t* task);
void manage_sleeping_tasks();
void insert_sleeping_list(unsigned long time);
void task_cleaner();
void task_cleanup(tcb_t* task);
void task_list_insert_front(task_list_t list, tcb_t* task);
void task_list_insert_back(task_list_t list, tcb_t* task);

/* Scheduler */
void schedule();
tcb_t* schedule_priority_task(tcb_t* list);
tcb_t* schedule_time_slice_task();

uint32_t* create_address_space();
uint32_t* relocate_stack(uint32_t* address, size_t size);
uint32_t get_next_pid();

tcb_t* create_task_handler(uint8_t state, uint32_t* address_space, uint32_t eip, void* params, uint8_t policy, uint8_t type);
process_t* create_process_handler(uint8_t state, uint32_t* address_space, uint32_t eip, void* params, uint8_t policy);
thread_t* create_thread_handler(uint8_t state, uint32_t eip, void* params, uint8_t policy);
//tcb_t* find_task(uint32_t pid);

#endif
