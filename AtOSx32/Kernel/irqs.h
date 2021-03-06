#ifndef IRQS_H
#define IRQS_H

#include "interrupts.h"
#include "hal.h"

#define EOI 0x20

enum PIC_TYPES {
  MASTER_COMMAND = 0x20,
  MASTER_DATA    = 0x21,
  SLAVE_COMMAND  = 0xA0,
  SLAVE_DATA     = 0xA1
};


void irq_install_handler(uint8_t irq, void(*handler)(isr_stack* stack));
void irq_remove_handler(uint8_t irq);
void irq_remap();
void init_irq();
void irq_handler(isr_stack* stack);




#endif
