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

void thread() {

  for(;;) {
    PRINT("HELLO");
    sleep(1000);
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
 
  tss_install();

  setup_multitasking();
  setup_clock();
  init_multitasking();


  tcb_t* task = create_process_handler(create_address_space(), (uint32_t)thread, NULL, POLICY_0);

  for (;;) {
    schedule();
  } 
  
 
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

