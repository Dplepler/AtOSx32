#ifndef PAGING_H
#define PAGING_H

#include <stdbool.h>
#include <stdint.h>

#define PD_ADDRESS 0x1000   // CHANGE THIS
#define PT_OFFSET  0x1000   // CHANGE THIS

unsigned long* page_physical_address(unsigned long* virtual_addr);
bool page_unmap(unsigned long* addr);
bool page_is_empty(unsigned long* pt_addr);
void pd_remove_empty_pt(unsigned long* pt_addr, unsigned long pd_index);


#endif