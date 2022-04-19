#ifndef PMM_H
#define PMM_H

#include <stdbool.h>
#include <stdint.h>
#include "hal.h"
#include "kernel_screen.h"

#define KERNEL_PHYSICAL 0x100000

#define NPAGES 0x100000       // Pages in RAM
#define BITS_IN_LONG 32
#define BITS_IN_BYTE 8

#define FREE_PAGES_OFFSET 0x1000

#define CHECK_FREE_FRAME(bitmap, i) (!CHECK_USED_FRAME(bitmap, i))
#define CHECK_USED_FRAME(bitmap, i) (bitmap[(uint32_t)i / BITS_IN_LONG] &   (1 << ((uint32_t)i % BITS_IN_LONG)))
#define MARK_USED(bitmap, i)        (bitmap[(uint32_t)i / BITS_IN_LONG] |=  (1 << ((uint32_t)i % BITS_IN_LONG)))
#define MARK_UNUSED(bitmap, i)      (bitmap[(uint32_t)i / BITS_IN_LONG] &= ~(1 << ((uint32_t)i % BITS_IN_LONG)))

#define CHANGE_PD(pd) asm("mov %0, %%eax\nmov %%eax, %%cr3":: "m"(pd))

#define MAX_PAGES_ALLOCATED 20

typedef uint32_t pgulong_t;

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

void bitmap_mark_kernel();
pgulong_t* palloc();
pgulong_t* palloc_single();
void pallocn(pgulong_t** frames, size_t size);



#endif