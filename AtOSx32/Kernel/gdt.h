#ifndef GDT_H
#define GDT_H

#include <stdint.h>
#include "hal.h"

#define GDT_SIZE 6

typedef struct _GDT_DESCRIPTOR_STRUCT {

  uint16_t limit;
  uint16_t base_l;
  uint8_t base_m;
  uint8_t access;
  uint8_t flags_and_limit;
  uint8_t base_h;

} __attribute__((packed)) gdtDescriptor;

typedef struct _GDT_DESCRIPTOR_POINTER_STRUCT {

  uint16_t limit;
  uint32_t offset;

} __attribute__((packed)) gdtptr;

void setup_gdt();
void init_gdt();
void load_gdt();
void gdt_create_gate(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);
void gdt_install_gates();

#endif