#include "clock.h"
#include "kernel_screen.h"

static unsigned long counter = 0;

void setup_clock() {
  set_periodic_interrupt();
}

/* Set clock to trigger IRQ8 */
void set_periodic_interrupt() {

  cli();
  outportb(CMOS_REGISTER, 0x8B);      // Select register 0xB and disable NMI e
  uint8_t old_data = inportb(CMOS_RW);
  outportb(CMOS_REGISTER, 0x8B);      // For some reason reading from this port resets the CMOS register to 0xD
  outportb(CMOS_RW, old_data | 0x40);
  sti();
}

/* Called 1024 times a second, keep track of the system's time */
void rtc_handler(isr_stack* stack) {

  stack = stack;    // Get rid of unused variable warning

  counter++; 

  /* To make sure a next IRQ8 will happen, read from the 0xC register */
  outportb(CMOS_REGISTER, 0xC);  
  inportb(CMOS_RW);
}

/* Delay the systems by the given miliseconds */
void sleep(unsigned long milisec) {
  unsigned long prev = counter;
  while (counter != prev + HERTZ(milisec)) { }
}

/* The system's timer. First time it's called it will initialize the timer and the 
   second time will return the time passed from the first call in miliseconds */
unsigned long clock_time() {
  
  static unsigned long count = 0;
  static unsigned long activated = false;

  if (!activated) { count = counter; }
  else { activated = false; return counter - count;  }

  activated = true;
  return ~0;
}

/* Check if the CMOS is currently busy */
bool cmos_update_in_progress() {
  outportb(CMOS_REGISTER, 0xA);
  return (bool)(inportb(CMOS_RW) & 0x80);
}

/* Get the current date and time using the rtc */
cmos_time rtc_get_time() {

  cmos_time rtc;

  outportb(CMOS_REGISTER, 0x0);
  rtc.seconds = inportb(CMOS_RW);
  outportb(CMOS_REGISTER, 0x2);
  rtc.minute = inportb(CMOS_RW);
  outportb(CMOS_REGISTER, 0x4);
  rtc.hour = inportb(CMOS_RW);
  outportb(CMOS_REGISTER, 0x7);
  rtc.day = inportb(CMOS_RW);
  outportb(CMOS_REGISTER, 0x8);
  rtc.month = inportb(CMOS_RW);
  outportb(CMOS_REGISTER, 0x9);
  rtc.year = inportb(CMOS_RW);

  return rtc;
}

/* Read the correct date and time from the rtc */
cmos_time read_rtc() {

  cmos_time rtc1;
  cmos_time rtc2;

  do {

    while (cmos_update_in_progress()) { }
    rtc1 = rtc_get_time();
    while (cmos_update_in_progress()) { }
    rtc2 = rtc_get_time();

  } while (rtc1.minute != rtc2.minute || rtc1.hour != rtc2.hour || rtc1.day != rtc2.day || rtc1.month != rtc2.month || rtc1.year != rtc2.year);
  
  outportb(CMOS_REGISTER, 0xB);
  uint8_t regb = inportb(CMOS_RW);

  /* If bcd is enabled, convert it to the binary format
     There is obviously no reason to use rtc2, 
     I just think it's funny */
  if (!(regb & 0b00000100)) { 
    rtc1.seconds = BCD_TO_BINARY(rtc2.seconds);
    rtc1.minute  = BCD_TO_BINARY(rtc2.minute);
    rtc1.hour    = BCD_TO_BINARY(rtc2.hour);
    rtc1.day     = BCD_TO_BINARY(rtc2.day);
    rtc1.month   = BCD_TO_BINARY(rtc2.month);
    rtc1.year    = BCD_TO_BINARY(rtc2.year);
  } 

  /* If the 12 hour mode was enabled, convert the format to the 24 hour format */
  if (!(regb & 0b00000010)) {
    rtc1.hour = rtc2.hour & 0x7F;
    if (rtc1.hour == 12) { rtc1.hour = 0; }   // Midnight is 0 in the 24 hour mode
  }

  /* Year is represented by two digits (e.g 22 is 2022, probably, maybe 1922), so we need to calculate the full year and mayne move up a century */
  rtc1.year += YEAR / 100 * 100;
  if (rtc1.year < YEAR) { rtc1.year += 100; }

  return rtc1;
}

char* date_to_string(cmos_time date) {
  
  char datestr[13]= { '\0' }; 

  

}

