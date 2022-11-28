#ifndef CLOCK_H
#define CLOCK_H

#include "System/hal.h"
#include "Tables/interrupts.h"
#include "Process/process.h"
#include "utils.h"

#define HERTZ(ms) (1024 * (ms / 1000.0))
#define BCD_TO_BINARY(bcd) (((bcd & 0xF0) >> 1) + ((bcd & 0xF0) >> 3) + (bcd & 0xF))

#define YEAR 2022

extern tcb_t* running_task;
extern task_list_t available_tasks[];

/*-------------------------------
Date formats (Register 0xB)     |
---------------------------     |
Bit 1 disabled: 12 hour format  |
Bit 1 enabled: 24 hour format   |
Bit 2 disabled: BCD mode        |
Bit 2 enabled: Binary mode      |
-------------------------------*/


typedef struct _CMOS_TIME_REGISTERS_STRUCT {

  uint8_t seconds;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint32_t year;
  
} cmos_time;

enum CMOS_PORTS {

  CMOS_REGISTER = 0x70,   // Pick register (and disable NMI)
  CMOS_RW       = 0x71    // Read/write to register
};

void setup_clock();
void set_periodic_interrupt();
void rtc_handler(isr_stack_t* stack);
void sleep(unsigned long sec);

cmos_time read_rtc();
cmos_time rtc_get_time();

bool cmos_update_in_progress();

char* dtoa(cmos_time date);
char* ttoa(cmos_time time);

unsigned long clock_time();

#endif

