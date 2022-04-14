#ifndef PAGING_H
#define PAGING_H

#include <stdbool.h>
#include <stdint.h>
#include "hal.h"

#define PD_ADDRESS 0xFFFFF000  
#define PD_CALC_ADDRESS 0xFFC00000  // Used when calculating with different addresses to get a page table 

bool pd_remove_entry(unsigned long* addr);
unsigned long* page_physical_address(unsigned long* virtual_addr);
bool page_unmap(unsigned long* addr);
bool page_is_empty(unsigned long* pt_addr);
void pd_remove_empty_pt(unsigned long* pt_addr, unsigned long pd_index);


#endif