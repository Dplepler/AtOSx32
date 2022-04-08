#include "hal.h"

uint8_t inportb(unsigned short port) {

   uint8_t ret;
   __asm__ __volatile__ ("inb %%dx,%%al":"=a" (ret):"d" (port));
   
	return ret;
}

void outportb(unsigned short port, uint8_t value) {
   
   __asm__ __volatile__ ("outb %%al,%%dx": :"d" (port), "a" (value));
}