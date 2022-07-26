#ifndef CLOCK_H
#define CLOCK_H

#include "hal.h"
#include "interrupts.h"

#define HERTZ(ms) (1024 * (ms / 1000))

enum CMOS_PORTS {

  CMOS_REGISTER = 0x70,   // Pick register (and disable NMI)
  CMOS_RW       = 0x71    // Read/write to register
};

void setup_clock();
void set_periodic_interrupt();
void rtc_handler(isr_stack* stack);
void sleep(unsigned long sec);

unsigned long clock_time();

#endif

