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
  //init_multitasking();

  init_fs();
  
  create_directory("dir", NULL, 0x0);

  create_directory("dir2", "dir", 0x0);
  
  create_file("aa.txt", "dir/dir2/", 0x0);
  char* txt = "OMG!!!";  

  inode_t* file = navigate_file("dir/dir2/aa.txt", NULL);
  write_file("dir/dir2/aa.txt", txt, strl(txt));
  
  inode_t* dir = navigate_dir("dir/dir2", NULL);

  copy_file(file, "dir");
 
  while(1) {}
  inode_t* copy = navigate_file("dir/aa.txt", NULL);
  PRINT(read_file(copy));

  /* init_cleaner_task();

  create_process_handler(create_address_space(), (uint32_t)clock, NULL, POLICY_0);
  create_process_handler(create_address_space(), (uint32_t)tongue, NULL, POLICY_0);


  while(1) { cli(); schedule(); sti(); } */


  return 0;
}

