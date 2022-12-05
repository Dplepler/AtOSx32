#include "Drivers/kernel_keyboard.h"
#include "Tables/gdt.h"
#include "Tables/interrupts.h"
#include "Tables/irqs.h"
#include "Memory/heap.h"
#include "System/clock.h"
#include "Tables/tss.h"
#include "Process/process.h"

extern tcb_t* running_task;
extern task_list_t** available_tasks;

extern task_list_t* sleeping_tasks;


void thread2(void* params) {

  PRINT("HELLO");
}

void thread3(void* params) {

  PRINT("WHATS UP");
}

void thread(void* params) {

  
  create_thread_handler((uint32_t)thread2, NULL, POLICY_0);
  create_thread_handler((uint32_t)thread3, NULL, POLICY_0);

  while(1) { //PRINTN(1); }
  }
}


void clock() {
  while (true) {
    terminal_draw_rec(0, 0, 9, 1, 8); 
    cursor_update(0, 0);
    cmos_time time = read_rtc();
    PRINT(ttoa(time));
    sleep(1000);
  }  
}

void tongue() {
  

  for (;;)  {
    terminal_draw_rec(31, 11, 49, 12, VGA_COLOR_LIGHT_RED);
    sleep(100);
    terminal_draw_rec(31, 11, 49, 12, VGA_COLOR_CYAN);
    sleep(100);
  }


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
 
  tss_install();

  setup_multitasking();
  setup_clock();
  init_multitasking();

  init_cleaner_task();
  

  create_process_handler(create_address_space(), (uint32_t)clock, NULL, POLICY_0);
  create_process_handler(create_address_space(), (uint32_t)tongue, NULL, POLICY_0);
  //create_process_handler(create_address_space(), (uint32_t)thread, NULL, POLICY_0);
  
  while(1) { cli(); schedule(); sti(); }
  

  return 0;
}

