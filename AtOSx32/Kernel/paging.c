#include "paging.h"


static uint32_t frame_bitmap[NPAGES / BITS_IN_LONG];

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
pgulong_t* page_get_table_address(pgulong_t pd_index) {
  return (pgulong_t*)(*((pgulong_t*)PD_CALC_ADDRESS + (pd_index * 4)));
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

void bitmap_mark_kernel() {

  size_t kernel_pages = (pgulong_t)(((uint32_t)&_kernel_end) - ((uint32_t)&_kernel_start)) / 0x1000;      
  const uint32_t kernel_first_index = ((uint32_t)&_kernel_start) / 0x1000;    

  /* Mark all kernel's page frames as used in the bitmap */
  for (uint32_t i = 0; i <= kernel_pages; i++) {
    MARK_USED(frame_bitmap, (kernel_first_index + i));
  }
}

/*
palloc_single allocates a single free page and marks it as used
Output: Unused page frame address
*/
pgulong_t* palloc_single() {

  uint32_t i = 0;

  /* Search for a free page */
  /* We calculate 32 bits each time, so we can easily skip big chunks of used memory */
  for (; i < NPAGES; i++) {
    if (frame_bitmap[i] ^ ~0 && CHECK_FREE_FRAME(frame_bitmap, i)) { break; }   
  }

  if (i == NPAGES) { PANIC("You ran out of RAM :("); }

  MARK_USED(frame_bitmap, i);

  return (pgulong_t*)(i * 0x1000);     // Return free page frame
}

/* 
pallocn allocates a specified amount of page frames 
in addition it will mark these pages as used

Input: Frame address array, amount to allocate
*/
void pallocn(pgulong_t** frames, size_t size) {
  for (size_t i = 0; i < size; i++) {
    frames[i] = palloc_single();
  }
}

/*
palloc allocates pages and returns their offsets

This function will allocate a chunk of pages each time to increase performance
*/
pgulong_t* palloc() {

  static bool allocate = true;
  static uint8_t pallocated = 0;
  static pgulong_t* current_frames[MAX_PAGES_ALLOCATED];

  if (pallocated == MAX_PAGES_ALLOCATED) { allocate = true; }
  
  /* Allocate 20 pages */
  if (allocate) { 
    pallocn(current_frames, MAX_PAGES_ALLOCATED); 
    pallocated = allocate = 0;    
  }

  pallocated++;
  return current_frames[pallocated - 1];
}


bool page_map(pgulong_t* paddr, pgulong_t* vaddr, uint16_t flags) {

  pgulong_t pd_index = pd_get_entry_index(vaddr);
  pgulong_t pt_index = page_get_entry_index(vaddr);

  pgulong_t* pt_addr = (((pgulong_t*)PD_ADDRESS)[pd_index] & 1) ? page_get_table_address(pd_index) 
    : pd_assign_table(pd_index);

  if ((pgulong_t)pt_addr[pt_index] & 1) { return false; }   // If page was already mapped, fail

  pt_addr[pt_index] = (pgulong_t)paddr | (flags & 0xFFF) | 1;

  flush_tlb_single(&pt_addr[pt_index]);

  return true;
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

pgulong_t* pd_assign_table(pgulong_t pd_index) {

  pgulong_t* pt_addr = palloc();
  ((pgulong_t*)PD_ADDRESS)[pd_index] = ((pgulong_t)pt_addr | 1);

  return pt_addr;
}

/*
Check if a page table is empty
Input: Page table offset address
Output: True if page table is empty, otherwise false
*/
bool page_is_empty(pgulong_t* pt_addr) {

  for (uint16_t i = 0; i < 0x400; i++) {
    if (pt_addr[i]) { return false; }
  }

  return true;
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

  for (uint16_t i = 0; i < 0x400; i++) {
    flush_tlb_single((pgulong_t*)pt_addr[i]);
  }
}

/*
page_is_aligned checks if an address is page aligned (4k aligned)
*/
bool page_is_aligned(pgulong_t* addr) {
  return !((pgulong_t)addr << 20);
}
