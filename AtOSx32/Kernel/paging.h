#ifndef PAGING_H
#define PAGING_H

#include <stdbool.h>
#include <stdint.h>

#define PD_ADDRESS 0x00001000           // Virtual address of the page directory
#define PT_OFFSET  PD_ADDRESS + 0x1000  // The page table offset comes right after the page directory

unsigned long* page_physical_address(unsigned long* virtual_addr);
bool page_unmap(unsigned long* addr);
bool page_is_empty(unsigned long* pt_addr);
void pd_remove_empty_pt(unsigned long* pt_addr, unsigned long pd_index);


#endif