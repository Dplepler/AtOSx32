#ifndef PAGING_H
#define PAGING_H

#include <stdbool.h>
#include <stdint.h>
#include "hal.h"
#include "kernel_screen.h"

#define PD_ADDRESS      0xFFFFF000  
#define PD_CALC_ADDRESS 0xFFC00000  // Used when calculating with different addresses to get a page table 

#define CHECK_FREE_FRAME(i) (i == 0)
#define MARK_USED(frame) (frame = 1)
#define NPAGES 0xFFEFF

#define MAX_PAGES_ALLOCATED 20

typedef uint32_t pgulong_t;
extern uint32_t _kernel_end;

pgulong_t* palloc();

pgulong_t* page_physical_address(pgulong_t* addr);
pgulong_t page_get_entry_index(pgulong_t* addr);
pgulong_t* page_get_table_address(pgulong_t pd_index);
bool page_unmap(pgulong_t* addr);
bool page_is_empty(pgulong_t* pt_addr);
bool page_is_aligned(pgulong_t* addr);

pgulong_t pd_get_entry_index(pgulong_t* addr);
bool pd_remove_entry(pgulong_t* addr);
void pd_remove_empty_pt(pgulong_t* pt_addr, pgulong_t pd_index);
void pd_flush_tlb(pgulong_t pd_index);


#endif