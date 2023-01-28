#ifndef IRQS_H
#define IRQS_H

#include "Tables/idt.h"

#define EOI 0x20

#define SYSTEM_IRQ_DISPATCHER 0x45

extern void syscall_dispatcher();

extern void create_file_handler(uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4, uint32_t param5);
extern void print(char* str);
extern void scheduler_tick();

enum PIC_TYPES {

  MASTER_COMMAND = 0x20,
  MASTER_DATA    = 0x21,
  SLAVE_COMMAND  = 0xA0,
  SLAVE_DATA     = 0xA1
};


void init_irq();
void irq_enable();
void irq_disable();
void init_syscalls(); 
void irq_install_handler(uint8_t irq, void(*handler)(isr_stack_t* stack));
void irq_remove_handler(uint8_t irq);
void irq_remap();
void init_irq();
void irq_handler(isr_stack_t* stack);

#endif

