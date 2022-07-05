#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

typedef struct INTERRUPT_DESCRIPTOR_STRUCT {

  uint16_t offset_lh;
  uint16_t selector;
  uint8_t unused;
  uint8_t attributes;
  uint16_t offset_hh;

} InterruptDescriptor;


#endif