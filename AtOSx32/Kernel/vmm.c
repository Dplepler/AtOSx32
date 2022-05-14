#include "vmm.h"

/*
pd_get_entry_index returns a page directory index from a given virtual address
*/
pgulong_t pd_get_entry_index(pgulong_t* addr) {
  return (pgulong_t)addr >> 22;
}

/*
page_get_entry index returns a page table index from a given virtual address
*/
pgulong_t page_get_entry_index(pgulong_t* addr) {
  return (pgulong_t)addr >> 12 & 0x3FF;
}

/*
page_get_table_address returns the address of a page table from a given page directory index
*/
pgulong_t* page_get_table_address(uint16_t pd_index) {
  return (pgulong_t*)PD_OFFSET + (pd_index * 4);
}

/*
page_physical_address converts a virtual address to a physical one
*/
pgulong_t* page_physical_address(pgulong_t* addr) {

  pgulong_t pd_index = pd_get_entry_index(addr);
  pgulong_t pt_index = page_get_entry_index(addr);

  pgulong_t* pt_addr = page_get_table_address(pd_index);

  return (pgulong_t*)((pt_addr[pt_index] & ~0xFFF) + ((pgulong_t)addr & 0xFFF));   
}

/*
pd_remove_entry removes a page table from the page directory
Input: Page entry's virtual address, note that an offset to the page table can be given and will be ignored'
Output: True if succeeded, otherwise false
*/
bool pd_remove_entry(pgulong_t* addr) {

  if (!page_is_aligned(addr)) { return false; }   // Address should always be page aligned

  pgulong_t pd_index = pd_get_entry_index(addr);  // Page directory index

  pd_flush_tlb(pd_index);

  ((pgulong_t*)PD_ADDRESS)[pd_index] = 0x2;       // Mark as unused

  return true;
}

/*
page_get_free_entry_index returns the first unused index in a given table
Input: Page table address
Output: Free entry's index, if there are none, function returns -1
*/
uint16_t page_get_free_entry_index(pgulong_t* table_addr) {

  for (uint16_t i = 0; i < ENTRIES; i++) {
    if (!(table_addr[i] & 1)) { return i; }
  }

  return ~0;
}

pgulong_t* page_get_free_addr() {

  uint16_t pt_index = 0;
  uint16_t i = 0;
  for (; i < ENTRIES; i++) {
    if ((pt_index = page_get_free_entry_index(&((pgulong_t*)PD_OFFSET)[i])) != ~0) { break; }
  }

  if (pt_index == ~0) { return NULL; }

  pgulong_t addr = (pgulong_t)i << 22;
  addr |= pt_index << 12;

  return (pgulong_t*)addr;
}

/*
page_map maps a physical address to a desired virtual address
Input: Desired virtual address and entry flags, if no specific address is desired, parameter can be NULL

Output: Mapped address, NULL if failed
*/
pgulong_t* page_map(pgulong_t* addr, uint16_t flags) {

  if (!addr) { addr = page_get_free_addr(); }

  pgulong_t pd_index = pd_get_entry_index(addr);
  pgulong_t pt_index = page_get_entry_index(addr);

  pgulong_t* pt_addr = (((pgulong_t*)PD_ADDRESS)[pd_index] & 1) ? page_get_table_address(pd_index) 
    : pd_assign_table(pd_index, flags);

  if ((pgulong_t)pt_addr[pt_index] & 1) { return NULL; }   // If page table index was already mapped, fail
  
  pt_addr[pt_index] = (pgulong_t)palloc();
  pt_addr[pt_index] |= (flags & 0xFFF) | 1;

  flush_tlb_single(&pt_addr[pt_index]);

  return addr;
}


/*
page_unmap unmaps a page
Input: Page frame's virtual address to unmap
Output: True if succeeded, otherwise false
*/
bool page_unmap(pgulong_t* addr) {

  if (!page_is_aligned(addr)) { return false; }        // Address should always be page aligned

  pgulong_t pd_index = pd_get_entry_index(addr);       // Page directory index
  pgulong_t pt_index = page_get_entry_index(addr);     // Page table index

  pgulong_t* pt_addr = page_get_table_address(pd_index);

  pt_addr[pt_index] = 0x0;    // Unmap entry

  pd_remove_empty_pt(pt_addr, pd_index);  // Remove entire page table from page directory if it's empty

  flush_tlb_single(&pt_addr[pt_index]);   // Flush TLB to recognize changes

  return true;
}

/*
pd_assign_table assigns a newly allocated page table to a page directory
Input: Page directory index to add page table to, flags to initialize with
Output: Address of new page table
*/
pgulong_t* pd_assign_table(pgulong_t pd_index, uint16_t flags) {

  pgulong_t* pt_phys_addr = palloc();
  ((pgulong_t*)PD_ADDRESS)[pd_index] = ((pgulong_t)pt_phys_addr | (flags & 0xFFF) | 1);

  pgulong_t* pt_addr = page_get_table_address(pd_index);
  page_clean(pt_addr);

  return pt_addr;
}

/*
Check if a page table is empty
Input: Page table offset address
Output: True if page table is empty, otherwise false
*/
bool page_is_empty(pgulong_t* pt_addr) {

  for (uint16_t i = 0; i < ENTRIES; i++) {
    if (pt_addr[i] & 1) { return false; }
  }

  return true;
}

void page_clean(pgulong_t* addr) {

  if (!page_is_aligned(addr)) { return; }

  for (uint16_t i = 0; i < ENTRIES; i++) {
    *(addr + i) = 0x0;
  }

  flush_tlb_single(addr);
}

/*
Check if page table is empty, if so remove it from the page directory
Input: Page table's address to check, Page directory index to remove from
*/
void pd_remove_empty_pt(pgulong_t* pt_addr, pgulong_t pd_index) {

  if (page_is_empty(pt_addr)) {
    ((pgulong_t*)PD_ADDRESS)[pd_index] = 0x2;   // Set entry as not present
  }
}

/*
pd_flush_tlb flushes all page directory entries
Input: Page directory entry index
*/
void pd_flush_tlb(pgulong_t pd_index) {

  pgulong_t* pt_addr = page_get_table_address(pd_index);
  
  for (uint16_t i = 0; i < ENTRIES; i++) {
    flush_tlb_single((pgulong_t*)pt_addr[i]);
  }
}

/*
page_is_aligned checks if an address is page aligned (4k aligned)
*/
bool page_is_aligned(pgulong_t* addr) {
  return !((pgulong_t)addr << 20);
}
