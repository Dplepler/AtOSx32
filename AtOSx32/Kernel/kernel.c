#include "Tables/gdt.h"
#include "Tables/interrupts.h"
#include "Tables/irqs.h"
#include "Memory/heap.h"
#include "System/clock.h"
#include "Tables/tss.h"
#include "Process/process.h"
#include "Drivers/ata.h"
#include "fs/fs.h"


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
    terminal_draw_rec(34, 11, 46, 12, VGA_COLOR_LIGHT_RED);
    sleep(750);
    terminal_draw_rec(34, 11, 46, 12, VGA_COLOR_CYAN);
    sleep(750);
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

  init_fat();
  //unsigned char buffer[512];
  //memsetw(buffer, 0x9999, 512);
  
  //ata_write(0, 1, buffer);
  //ata_write(1, 1, buffer);
  //ata_write(2, 1, buffer);
  //ata_write(3, 1, buffer);


  
  //init_cleaner_task();

  /* create_process_handler(create_address_space(), (uint32_t)clock, NULL, POLICY_0);
  create_process_handler(create_address_space(), (uint32_t)tongue, NULL, POLICY_0); */


  //while(1) { cli(); schedule(); sti(); }


  return 0;
}

