#ifndef VMM_H
#define VMM_H

#include "Memory/pmm.h"
#include "System/errors.h"


#define PD_ADDRESS      0xFFFFF000  
#define PD_OFFSET       0xFFC00000  // Used when calculating with different addresses to get a page table 

#define PD_SIZE         0x1000
#define PT_SIZE         0x1000
#define PD_ENTRIES      0x400       // Amount of table entries

#define KERNEL_ENTRY_OFFSET       0x300     // Offset to higher half kernel in the pd
#define KERNEL_PAGE_TABLE_OFFSET  0x1002000  
enum PAGE_FLAGS {

  PRESENT         =  0b00000001,
  READ_WRITE      =  0b00000010,
  USER_ACCESS     =  0b00000100,
  WRITE_THROUGH   =  0b00001000,
  NO_CACHE        =  0b00010000,
  ACCESSED        =  0b00100000,
  DIRTY           =  0x01000000,
  ATTRIBUTE_TABLE =  0b10000000,
  GLOBAL          = 0b100000000,

};

void* memset(void* addr, uint8_t c, size_t n);
void  memcpy(void* s1, const void* s2, size_t n);

pgulong_t* page_physical_address(pgulong_t* addr);
pgulong_t* page_get_table_address(uint16_t pd_index);
pgulong_t* page_get_free_addr(size_t length, int* err);
pgulong_t* page_make_address(pgulong_t pd_index, pgulong_t pt_index);
pgulong_t* page_map(pgulong_t* addr, size_t length, uint16_t flags);
pgulong_t* pd_assign_table(pgulong_t pd_index, uint16_t flags);
pgulong_t* page_memory_under4mb(size_t length, int* err);
pgulong_t* page_memory_above4mb(size_t length, int* err);

pgulong_t page_get_entry_index(pgulong_t* addr);
pgulong_t pd_get_entry_index(pgulong_t* addr);

bool page_unmap(pgulong_t* addr, size_t length);
bool page_is_empty(pgulong_t pd_index);
bool page_is_aligned(pgulong_t* addr);
bool pd_remove_entry(pgulong_t* addr);

void map_higher_half(pgulong_t* address_space);
void pd_remove_empty_pt(pgulong_t pd_index);
void pd_flush_tlb(pgulong_t pd_index);
void page_clean(pgulong_t* addr);

#endif

