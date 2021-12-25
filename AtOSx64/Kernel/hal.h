#ifndef HAL_H
#define HAL_H

#include <stddef.h>
#include <stdint.h>

uint8_t inportb(unsigned short port);
void outportb(unsigned short port, uint8_t value);

#endif