#include "clock.h"
#include "kernel_screen.h"

static unsigned long counter = 0;

void setup_clock() {
  set_periodic_interrupt();
}

/* Set clock to trigger IRQ8 */
void set_periodic_interrupt() {

  cli();
  outportb(CMOS_REGISTER, 0x8B);      // Select register 0xB and disable NMI
  uint8_t old_data = inportb(CMOS_RW);
  outportb(CMOS_REGISTER, 0x8B);      // For some reason reading from this port resets the CMOS register to 0xD
  outportb(CMOS_RW, old_data | 0x40);
  sti();
}


void rtc_handler(isr_stack* stack) {

  counter++; 

  /* To make sure a next IRQ8 will happen, read from the 0xC register */
  outportb(CMOS_REGISTER, 0xC);  
  inportb(CMOS_RW);
}


void sleep(unsigned long milisec) {
  unsigned long prev = counter;
  while (counter != prev + HERTZ(milisec)) { }
}

unsigned long clock_time() {
  
  static unsigned long count = 0;
  static unsigned long activated = false;

  if (!activated) { count = counter; }
  else { activated = false; return counter - count;  }

  activated = true;
  return ~0;
}
