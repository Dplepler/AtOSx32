#include "paging.h"

bool page_unmap(unsigned long* addr) {

    if (((unsigned long)addr << 20)) { return false; }          // Address should always be page aligned

    unsigned long pd_index = (unsigned long)addr >> 22;         // Page directory index
    unsigned long pt_index = (unsigned long)addr >> 12 & 0x3FF; // Page table index

    unsigned long* pt_addr = (unsigned long*)PAGE_TABLES_ADDR + (pd_index * 0x400);  // Get page table address

    pt_addr[pt_index] = 0x0;    // Unmap entry

    pd_remove_empty_pt(pt_addr, pd_index);

    return true;
}

/*
Check if a page table is empty
Input: Page table offset address
Output: True if page table is empty, otherwise false
*/
bool page_is_empty(unsigned long* pt_addr) {

    for (unsigned int i = 0; i < 0x400; i += 4) {

        if (pt_addr[i]) { return false; }
    }

    return true;
}

/*
Check if page table is empty, if so remove it from the page directory
Input: Page table's address to check, Page directory index to remove from
*/
void pd_remove_empty_pt(unsigned long* pt_addr, unsigned long pd_index) {

    if (page_is_empty(pt_addr)) {

        ((unsigned long*)DIRECTORY_ADDR)[pd_index] = 0x2;   // Set entry as not present
    }
}
