#ifndef PAGING_H
#define PAGING_H

#include <stdbool.h>
#include <stdint.h>

#define DIRECTORY_ADDR 0xC03FF000
#define PAGE_TABLES_ADDR 0xFFC00000

bool page_unmap(unsigned long* addr);
bool page_is_empty(unsigned long* pt_addr);
void pd_remove_empty_pt(unsigned long* pt_addr, unsigned long pd_index);


#endif