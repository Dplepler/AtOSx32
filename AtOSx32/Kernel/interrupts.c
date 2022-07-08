#include "interrupts.h"

static InterruptDescriptor idt[IDT_SIZE];
static idtptr idt_ptr;

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

void init_idt() {
  idt_ptr.limit = sizeof(InterruptDescriptor) * 256 - 1;
  memset(idt, 0, sizeof(InterruptDescriptor) * 256);
  idt_ptr.offset = idt;
  asm ("lidt (%0)\n" :: "r" (&idt_ptr));
  PRINTNH(&idt_ptr);
}

void load_idt() {
  // PRINTNH(&idt_ptr);
  // cpu_load_idt(&idt_ptr);
}

void idt_create_gate(uint8_t index, uint32_t address, uint16_t select, uint8_t attributes) {

  idt[index].attributes = attributes;
  idt[index].offset_lh  = (uint16_t)(address & 0xFFFF);
  idt[index].offset_hh  = (uint16_t)(address >> 16);
  idt[index].selector   = select;
}

void idt_install_gates() {
  
  idt_create_gate(0,  (unsigned)isr0,  0x8, IDT_GATE);
  idt_create_gate(1,  (unsigned)isr1,  0x8, IDT_GATE);
  idt_create_gate(2,  (unsigned)isr2,  0x8, IDT_GATE);
  idt_create_gate(3,  (unsigned)isr3,  0x8, IDT_GATE);
  idt_create_gate(4,  (unsigned)isr4,  0x8, IDT_GATE);
  idt_create_gate(5,  (unsigned)isr5,  0x8, IDT_GATE);
  idt_create_gate(6,  (unsigned)isr6,  0x8, IDT_GATE);
  idt_create_gate(7,  (unsigned)isr7,  0x8, IDT_GATE);
  idt_create_gate(8,  (unsigned)isr8,  0x8, IDT_GATE);
  idt_create_gate(9,  (unsigned)isr9,  0x8, IDT_GATE);
  idt_create_gate(10, (unsigned)isr10, 0x8, IDT_GATE);
  idt_create_gate(11, (unsigned)isr11, 0x8, IDT_GATE);
  idt_create_gate(12, (unsigned)isr12, 0x8, IDT_GATE);
  idt_create_gate(13, (unsigned)isr13, 0x8, IDT_GATE);
  idt_create_gate(14, (unsigned)isr14, 0x8, IDT_GATE);
  idt_create_gate(15, (unsigned)isr15, 0x8, IDT_GATE);
  idt_create_gate(16, (unsigned)isr16, 0x8, IDT_GATE);
  idt_create_gate(17, (unsigned)isr17, 0x8, IDT_GATE);
  idt_create_gate(18, (unsigned)isr18, 0x8, IDT_GATE);
  idt_create_gate(19, (unsigned)isr19, 0x8, IDT_GATE);
  idt_create_gate(20, (unsigned)isr20, 0x8, IDT_GATE);
  idt_create_gate(21, (unsigned)isr21, 0x8, IDT_GATE);
  idt_create_gate(22, (unsigned)isr22, 0x8, IDT_GATE);
  idt_create_gate(23, (unsigned)isr23, 0x8, IDT_GATE);
  idt_create_gate(24, (unsigned)isr24, 0x8, IDT_GATE);
  idt_create_gate(25, (unsigned)isr25, 0x8, IDT_GATE);
  idt_create_gate(26, (unsigned)isr26, 0x8, IDT_GATE);
  idt_create_gate(27, (unsigned)isr27, 0x8, IDT_GATE);
  idt_create_gate(28, (unsigned)isr28, 0x8, IDT_GATE);
  idt_create_gate(29, (unsigned)isr29, 0x8, IDT_GATE);
  idt_create_gate(30, (unsigned)isr30, 0x8, IDT_GATE);
  idt_create_gate(31, (unsigned)isr31, 0x8, IDT_GATE);
}

void fault_handler(isr_stack* stack) {

  PANIC("ERNJFNJGR");
  if (stack->index > 31) { return; }

  /* Exceptions */
  char* exceptions[] =  {

    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
  };

  PANIC(exceptions[stack->index]);
}


