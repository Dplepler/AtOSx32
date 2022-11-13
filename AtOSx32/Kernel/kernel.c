#include "Drivers/kernel_keyboard.h"
#include "Tables/gdt.h"
#include "Tables/interrupts.h"
#include "Tables/irqs.h"
#include "Memory/heap.h"
#include "System/clock.h"
#include "Tables/tss.h"
#include "Process/process.h"

void test(int x) {

  PRINT("TEST SUCCESS!");
  PRINTNH(x); 
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

  //PRINTNH(terminate_process); 
  
  run_task(create_process(TASK_ACTIVE, create_address_space(), (uint32_t)hello));
  
  //test(10);
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

