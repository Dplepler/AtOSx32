#include "hal.h"

uint8_t inportb(unsigned short port)
{
   	uint8_t ret = 10;
   	asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));

   	return ret;
}

void outportb(unsigned short port, uint8_t value)
{
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}