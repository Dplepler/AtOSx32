#ifndef PAGING_H
#define PAGING_H

#include <stdbool.h>
#include <stdint.h>
#include "hal.h"
#include "kernel_screen.h"

#define PD_ADDRESS      0xFFFFF000  
#define PD_CALC_ADDRESS 0xFFC00000  // Used when calculating with different addresses to get a page table 

#define NPAGES 0x100000             // Pages in RAM
#define BITS_IN_LONG 32
#define BITS_IN_BYTE 8

#define CHECK_FREE_FRAME(bitmap, i) (!CHECK_USED_FRAME(bitmap, i))
#define CHECK_USED_FRAME(bitmap, i) (bitmap[(uint32_t)i / BITS_IN_LONG] &   (1 << ((uint32_t)i % BITS_IN_LONG)))
#define MARK_USED(bitmap, i)        (bitmap[(uint32_t)i / BITS_IN_LONG] |=  (1 << ((uint32_t)i % BITS_IN_LONG)))
#define MARK_UNUSED(bitmap, i)      (bitmap[(uint32_t)i / BITS_IN_LONG] &= ~(1 << ((uint32_t)i % BITS_IN_LONG)))

#define CHANGE_PD(pd) asm("mov %0, %%eax\nmov %%eax, %%cr3":: "m"(pd))

#define MAX_PAGES_ALLOCATED 20

typedef uint32_t pgulong_t;

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

pgulong_t* palloc();
pgulong_t* palloc_single();
void pallocn(pgulong_t** frames, size_t size);
void bitmap_mark_kernel();

pgulong_t* page_physical_address(pgulong_t* addr);
pgulong_t page_get_entry_index(pgulong_t* addr);
pgulong_t* page_get_table_address(pgulong_t pd_index);
bool page_map(pgulong_t* paddr, pgulong_t* vaddr, uint16_t flags);
bool page_unmap(pgulong_t* addr);
bool page_is_empty(pgulong_t* pt_addr);
bool page_is_aligned(pgulong_t* addr);

pgulong_t* pd_assign_table(pgulong_t pd_index);
pgulong_t pd_get_entry_index(pgulong_t* addr);
bool pd_remove_entry(pgulong_t* addr);
void pd_remove_empty_pt(pgulong_t* pt_addr, pgulong_t pd_index);
void pd_flush_tlb(pgulong_t pd_index);


#endif