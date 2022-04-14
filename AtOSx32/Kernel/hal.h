#ifndef HAL_H
#define HAL_H

#include <stddef.h>
#include <stdint.h>

uint8_t inportb(uint16_t port);
void outportb(uint16_t port, uint8_t value);

#endif