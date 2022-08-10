#ifndef TSS_H
#define TSS_H

#include <stdint.h>
#include "gdt.h"

#define KERNEL_STACK_SIZE 0x1000 

typedef struct _TASK_STATE_SEGMENT_STRUCT {

  uint16_t link;
  uint16_t r1;
  uint32_t esp0;
  uint16_t ss0;
  uint16_t r2;
  uint32_t esp1;
  uint16_t ss1;
  uint16_t r3;
  uint32_t esp2;
  uint16_t ss2;
  uint16_t r4;
  uint32_t cr3;
  uint32_t eip;
  uint32_t eflags;
  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esi;
  uint32_t edi;
  uint16_t es;
  uint16_t r5;
  uint16_t cs;
  uint16_t r6;
  uint16_t ss;
  uint16_t r7;
  uint16_t ds;
  uint16_t r8;
  uint16_t fs;
  uint16_t r9;
  uint16_t gs;
  uint16_t r10;
  uint16_t ldtr;
  uint32_t r11;
  uint16_t iopb;
  uint32_t ssp;

} __attribute__((packed)) tss_t;


void tss_install();

#endif
