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

extern void idt_flush();

void setup_idt() {
  init_idt();
  load_idt();
  idt_install_gates();
}

void init_idt() {
  idt_ptr.limit = sizeof(InterruptDescriptor) * IDT_SIZE - 1;
  memset(idt, 0, sizeof(InterruptDescriptor) * IDT_SIZE);
  idt_ptr.offset = (uint32_t)&idt;
}

void load_idt() {
  cpu_load_idt(&idt_ptr);
}

void idt_create_gate(uint8_t index, uint32_t address, uint16_t select, uint8_t attributes) {
  idt[index].attributes = attributes;
  idt[index].offset_lh  = (uint16_t)(address & 0xFFFF);   // Address' high bits
  idt[index].offset_hh  = (uint16_t)(address >> 16);      // Address' low bits
  idt[index].selector   = select;
  idt[index].unused     = 0;
}

void idt_install_gates() {
  
  idt_create_gate(0,  (uint32_t)isr0,  0x8, IDT_GATE);
  idt_create_gate(1,  (uint32_t)isr1,  0x8, IDT_GATE);
  idt_create_gate(2,  (uint32_t)isr2,  0x8, IDT_GATE);
  idt_create_gate(3,  (uint32_t)isr3,  0x8, IDT_GATE);
  idt_create_gate(4,  (uint32_t)isr4,  0x8, IDT_GATE);
  idt_create_gate(5,  (uint32_t)isr5,  0x8, IDT_GATE);
  idt_create_gate(6,  (uint32_t)isr6,  0x8, IDT_GATE);
  idt_create_gate(7,  (uint32_t)isr7,  0x8, IDT_GATE);
  idt_create_gate(8,  (uint32_t)isr8,  0x8, IDT_GATE);
  idt_create_gate(9,  (uint32_t)isr9,  0x8, IDT_GATE);
  idt_create_gate(10, (uint32_t)isr10, 0x8, IDT_GATE);
  idt_create_gate(11, (uint32_t)isr11, 0x8, IDT_GATE);
  idt_create_gate(12, (uint32_t)isr12, 0x8, IDT_GATE);
  idt_create_gate(13, (uint32_t)isr13, 0x8, IDT_GATE);
  idt_create_gate(14, (uint32_t)isr14, 0x8, IDT_GATE);
  idt_create_gate(15, (uint32_t)isr15, 0x8, IDT_GATE);
  idt_create_gate(16, (uint32_t)isr16, 0x8, IDT_GATE);
  idt_create_gate(17, (uint32_t)isr17, 0x8, IDT_GATE);
  idt_create_gate(18, (uint32_t)isr18, 0x8, IDT_GATE);
  idt_create_gate(19, (uint32_t)isr19, 0x8, IDT_GATE);
  idt_create_gate(20, (uint32_t)isr20, 0x8, IDT_GATE);
  idt_create_gate(21, (uint32_t)isr21, 0x8, IDT_GATE);
  idt_create_gate(22, (uint32_t)isr22, 0x8, IDT_GATE);
  idt_create_gate(23, (uint32_t)isr23, 0x8, IDT_GATE);
  idt_create_gate(24, (uint32_t)isr24, 0x8, IDT_GATE);
  idt_create_gate(25, (uint32_t)isr25, 0x8, IDT_GATE);
  idt_create_gate(26, (uint32_t)isr26, 0x8, IDT_GATE);
  idt_create_gate(27, (uint32_t)isr27, 0x8, IDT_GATE);
  idt_create_gate(28, (uint32_t)isr28, 0x8, IDT_GATE);
  idt_create_gate(29, (uint32_t)isr29, 0x8, IDT_GATE);
  idt_create_gate(30, (uint32_t)isr30, 0x8, IDT_GATE);
  idt_create_gate(31, (uint32_t)isr31, 0x8, IDT_GATE);

  cpu_load_idt(&idt_ptr);
}

void fault_handler(isr_stack* stack) {

  if ((stack->index & 0xFF) > 31) { return; }

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

  PANIC(exceptions[stack->index & 0xFF]);
}


