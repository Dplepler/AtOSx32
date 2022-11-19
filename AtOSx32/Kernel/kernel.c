#include "Drivers/kernel_keyboard.h"
#include "Tables/gdt.h"
#include "Tables/interrupts.h"
#include "Tables/irqs.h"
#include "Memory/heap.h"
#include "System/clock.h"
#include "Tables/tss.h"
#include "Process/process.h"

extern tcb_t* running_task;

void thread1(void* args);
void thread2(void* args);
void process(void* args);

void process(void* args) {

  thread_t* child1 = create_thread_handler(TASK_ACTIVE, (uint32_t)thread1);
  thread_t* child2 = create_thread_handler(TASK_ACTIVE, (uint32_t)thread2);
  
  run_task(child1, child2);

  while(1) {}
}


void thread1(void* args) {

  PRINT("HELEGFRMHG");
  while(1) {}
  PRINT("HI from thread1\n\r");
  run_task(args, running_task);
  while(1) {}

}

void thread2(void* args) {

  PRINT("HI from thread2\n\r");
  while(1) {}
  run_task(args, running_task);
  while(1) {}
}

int kmain(void) {
  
  pd_remove_entry(0); 	// Remove identity mapping
  
  /* Initialize terminal interface */
  terminal_initialize();

  bitmap_mark_kernel();

  perry(25, 5);

  idtptr_t* idt_ptr = kmalloc(sizeof(idtptr_t));
  interrupt_descriptor_t** idt = kmalloc(IDT_SIZE * sizeof(interrupt_descriptor_t*));
  
  gdtptr* gdt_ptr = kmalloc(sizeof(gdtptr));
  gdt_descriptor** gdt = kmalloc(GDT_SIZE * sizeof(gdt_descriptor*));

  tss_t* tss = kmalloc(sizeof(tss_t));

  setup_gdt(gdt_ptr, gdt);
  setup_idt(idt_ptr, idt);
  init_irq(idt);
  setup_clock();
  
  tss_install(tss, gdt_ptr, gdt);

  init_multitasking();
  

  process_t* root = create_process_handler(TASK_ACTIVE, cpu_get_address_space(), (uint32_t)process);
  run_task(root, NULL);


  while(1) {}
  
  /* while (true) {
    terminal_draw_rec(0, 0, 9, 1, 8); 
    cursor_update(0, 0);
    cmos_time time = read_rtc();
    PRINT(ttoa(time));
    sleep(1000);
  } */ 

  return 0;
}

