#include "irqs.h"

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();


void* irq_routines[256] = { NULL };
void* service_routines[256] = { NULL };

void irq_install_handler(uint8_t irq, void(*handler)(isr_stack_t* stack)) {
  irq_routines[irq] = handler;
}

void irq_remove_handler(uint8_t irq) {
  irq_routines[irq] = NULL;
}

void init_syscalls() {
  
  idt_create_gate(SYSTEM_IRQ_DISPATCHER, (uint32_t)syscall_dispatcher, 0x8, 0xEE);
  
  service_routines[0] = &create_file_handler;

}

void syscall_dispatcher() {

  uint8_t index;
  __asm__ __volatile__ ("mov %%ah, %0" : "=r" (index));

  void* service = service_routines[index];

  __asm__ __volatile__ (" \
    push %%edi;     \
    push %%esi;     \
    push %%edx;     \
    push %%ecx;     \
    push %%ebx;     \
    call %0;        \
    add $20, %%esp; \
    iret;           "
    
    : : "r"((uint32_t)service));
}


/* The first 8 IRQs are by default mapped to entries 8-15, these entries are already reserved for exceptions with double faults
so we need to remap the IRQs via the Programmble Interrupt Controller; New location will be at entries 32-47 */
void irq_remap() {

  outportb(MASTER_COMMAND, 0x11);
  outportb(SLAVE_COMMAND,  0x11);
  outportb(MASTER_DATA,    0x20);
  outportb(SLAVE_DATA,     0x28);
  outportb(MASTER_DATA,       4);
  outportb(SLAVE_DATA,        2);
  outportb(MASTER_DATA,       1);
  outportb(SLAVE_DATA,        1);
  outportb(MASTER_DATA,       0);
  outportb(SLAVE_DATA,        0);
}

void init_irq() {
  
  irq_remap();

  idt_create_gate(32, (uint32_t)irq0, 0x8, IDT_GATE);
  idt_create_gate(33, (uint32_t)irq1, 0x8, IDT_GATE);
  idt_create_gate(34, (uint32_t)irq2, 0x8, IDT_GATE);
  idt_create_gate(35, (uint32_t)irq3, 0x8, IDT_GATE);
  idt_create_gate(36, (uint32_t)irq4, 0x8, IDT_GATE);
  idt_create_gate(37, (uint32_t)irq5, 0x8, IDT_GATE);
  idt_create_gate(38, (uint32_t)irq6, 0x8, IDT_GATE);
  idt_create_gate(39, (uint32_t)irq7, 0x8, IDT_GATE);
  idt_create_gate(40, (uint32_t)irq8, 0x8, IDT_GATE);
  idt_create_gate(41, (uint32_t)irq9, 0x8, IDT_GATE);
  idt_create_gate(42, (uint32_t)irq10, 0x8, IDT_GATE);
  idt_create_gate(43, (uint32_t)irq11, 0x8, IDT_GATE);
  idt_create_gate(44, (uint32_t)irq12, 0x8, IDT_GATE);
  idt_create_gate(45, (uint32_t)irq13, 0x8, IDT_GATE);
  idt_create_gate(46, (uint32_t)irq14, 0x8, IDT_GATE);
  idt_create_gate(47, (uint32_t)irq15, 0x8, IDT_GATE);
}



/* Handle a default interrupt request */
void irq_handler(isr_stack_t* stack) {

  uint32_t index = (stack->index & 0xFF) - 0x20;

  void (*handler)(isr_stack_t* stack) = irq_routines[index]; 
  
  if (handler) { (*handler)(stack); }

  /* If we are trying to access an IRQ that belongs to the second PIC, send an End of Interrupt command to it */
  if (stack->index >= 40) { outportb(SLAVE_COMMAND, EOI); }
  
  /* In any case send an End of Interrupt command to the first PIC */
  outportb(MASTER_COMMAND, EOI);

  /* Handle task switching */
  if (index == 0x8) { scheduler_tick(); } 
  else { sti(); }
}

