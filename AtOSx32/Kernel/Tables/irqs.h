#ifndef IRQS_H
#define IRQS_H

#include "Tables/idt.h"
#include "Process/process.h"

#define EOI 0x20

#define SYSTEM_IRQ_DISPATCHER 0x45

extern void create_file_handler(uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4, uint32_t param5);

enum PIC_TYPES {

  MASTER_COMMAND = 0x20,
  MASTER_DATA    = 0x21,
  SLAVE_COMMAND  = 0xA0,
  SLAVE_DATA     = 0xA1
};

extern tcb_t* scheduler_task;
extern tcb_t* running_task;
extern tcb_t* next_task;
extern bool allow_ts;

static inline void scheduler_tick() {

  allow_ts = false; 
  /* Wake up tasks */
  manage_sleeping_tasks();
    
  /* Decrease the tasks's time slice */
  if (scheduler_task != running_task) { manage_time_slice(); } 
  allow_ts = true;
}

void init_irq();
void init_syscalls(); 
void syscall_dispatcher();
void irq_install_handler(uint8_t irq, void(*handler)(isr_stack_t* stack));
void irq_remove_handler(uint8_t irq);
void irq_remap();
void init_irq();
void irq_handler(isr_stack_t* stack);

#endif

