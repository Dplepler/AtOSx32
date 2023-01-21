#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "fs/fs.h"
#include "Memory/heap.h"

#define INIT_KERNEL_STACK 0xC03FE000
#define STACK_SIZE        0x1000

#define DEFAULT_TIME_SLICE 50
#define DEFAULT_PRIORITY   128

#define MIN_DECREASED_PRIORITY 20

#define POLICY_AMOUNT 4


#define PT_LOAD   1

#define PROGRAM_X 1
#define PROGRAM_W 2
#define PROGRAM_R 4

extern unsigned long proc_time_counter; 
extern unsigned long time_counter;
extern unsigned long idle_time_counter;
extern void sleep(unsigned long milisec);
extern void jmp_userland(void* func);


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

  uint32_t time_slice;
  uint8_t priority;       // Only for policies 0 & 1
  uint8_t req_priority;

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


extern tcb_t* scheduler_task;
extern tcb_t* running_task;
extern bool allow_ts;

extern void switch_task(struct _TASK_CONTROL_BLOCK_STRUCT* new_task);

void setup_multitasking();
void init_multitasking();
void init_cleaner_task();
void process_startup(inode_t* code);
void run_task();
void terminate_task();
void init_task(tcb_t* task, void* params);
void update_proc_time();
void task_change_state(tcb_t* task, uint16_t state);
void task_block(uint32_t new_state);
void task_unblock(tcb_t* task);
void manage_sleeping_tasks();
void manage_time_slice();
void set_naptime(unsigned long time);
void task_cleaner();
void task_cleanup(tcb_t* task);
void task_list_insert_front(task_list_t* list, tcb_t* task);
void task_list_insert_back(task_list_t* list, tcb_t* task);
void task_list_remove_task(task_list_t* list, tcb_t* task);

/* Scheduler */
void schedule();
void sleep(unsigned long milisec);
void scheduler_tick(); 
void run_elf_file(elf32_header_t* fheader);

tcb_t* create_task_handler(uint32_t* address_space, uint32_t eip, void* params, uint8_t policy);
tcb_t* schedule_priority_task(tcb_t* list);
tcb_t* schedule_time_slice_task();

uint32_t* create_address_space();
uint32_t* relocate_stack(uint32_t* address, size_t size);
uint32_t get_next_pid();

process_t* create_process_handler(uint32_t* address_space, uint32_t eip, void* params, uint8_t policy);
thread_t* create_thread_handler(uint32_t eip, void* params, uint8_t policy);

#endif

