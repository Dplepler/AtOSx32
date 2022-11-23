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
 
  PRINTNH(args);
  for (;;) {
    PRINT("HI from thread1\n\r");
    while(1) {}
    run_task(args, running_task);
  }
 
  while(1) {}
}

void thread2(void* args) {

  for (;;) {
    while(1) {}
    PRINT("HI from thread2\n\r");
    while(1) {}
    run_task(args, running_task);
    
    while(1) {}
  }
  while(1) {}
}

int kmain(void) {
  
  pd_remove_entry(0); 	// Remove identity mapping
  
  /* Initialize terminal interface */
  terminal_initialize();

  bitmap_mark_kernel();

  perry(25, 5);

  setup_gdt();
  setup_idt();
  
  init_irq();

  setup_clock();
  
  tss_install();

  init_multitasking();
  
  NL;
  PRINT("THREAD 2: "); PRINTNH(thread2);
  NL;
  process_t* root = create_thread_handler(TASK_ACTIVE, (uint32_t)process);
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

