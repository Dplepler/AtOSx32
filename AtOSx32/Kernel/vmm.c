#include "vmm.h"

int counter = 0;

void* memset(void* addr, uint8_t c, size_t n) {

  for (uint32_t i = 0; i < n; i++) {
    ((char*)addr)[i] = c;
  }

  return addr;
}

void* memcpy(void* s1, const void* s2, size_t n) {

  for (; n >= sizeof(unsigned long); n -= sizeof(unsigned long)) {
    *(unsigned long*)s1++ = *(unsigned long*)s2++;
  }

  uint8_t* cdst = (uint8_t*)s1;
  uint8_t* csrc = (uint8_t*)s2;

  while (n > 0) {
    *cdst++ = *csrc++;
    n--;
  }

  return s1;
}

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

pgulong_t* page_make_address(pgulong_t pd_index, pgulong_t pt_index) {

  pgulong_t addr = pd_index << 22;
  addr |= pt_index << 12;

  return (pgulong_t*)addr;
}

int page_get_free_memory_index(size_t req_pd_entries, size_t req_pt_entries) {

  req_pt_entries -= (0x400 * req_pd_entries);

  pgulong_t* addr = NULL;

  for (uint16_t i = 0; i < ENTRIES; i++) {
    if (ENTRIES - i < (int)req_pd_entries) { break; }

    uint32_t i2 = i;
    for (; i2 < i + req_pd_entries; i2++) {
      if (i2 > ENTRIES) { return ~0; }
      addr = ((pgulong_t*)PD_ADDRESS)[i2];
      if (*addr & 1) { break; }                     // Bad
      if (i2 == i + req_pd_entries - 1) { break; }  // Good
    }

    if (i2 == i + req_pd_entries - 1) {
      if (!req_pt_entries) { return (int)i; }   // We're done
      if (i2 == ENTRIES) { break; }             // Not enough space for extra page table entries
      addr = ((pgulong_t*)PD_ADDRESS)[i2 + 1];  // Next page directory
      if (!((pgulong_t)addr & 1)) { return (int)i; }  // PD is empty, we can use it
      for (uint32_t i3 = 0; i3 < req_pt_entries; i3++) {
        if (addr[i3] & 1) { break; }                        // Bad
        if (i3 == req_pt_entries - 1) { return (int)i; }    // Good
      }
    }
  }

  return ~0;
}

/*
page_get_free_pt_memory_index returns an address that contains req_pt_entries * 4kb of free contigious memory
Input: Required amount of page table entries (n bytes * 4kb of wanted memory)
Output: Virtual address containing the offset of the free memory, err will contain the error if there is one
*/
pgulong_t* page_get_free_pt_memory_index(size_t req_pt_entries, int* err) {

  pgulong_t* addr = NULL;

  for (uint16_t i = 0; i < ENTRIES; i++) {
    addr = page_get_table_address(i);
    if (page_is_empty(i)) { return page_make_address(i, 0); }
    for (uint16_t i2 = 0; i2 + req_pt_entries <= ENTRIES; i2++) {
      for (uint16_t i3 = i2; i3 < i2 + req_pt_entries; i3++) {
        if (addr[i3] & 1) { i2 = i3; break; }
        if (i3 == i2 + req_pt_entries - 1) { return page_make_address(i, i2); }
      }
    }
  }

  *err = NOT_ENOUGH_SPACE;
  return NULL;
}

pgulong_t* page_get_free_addr(size_t length, int* err) {

  int index = 0;

  size_t req_pd_entries = length / 0x400000;

  size_t req_pt_entries = length / 0x1000;
  if (length % 0x1000) { req_pt_entries++; }

  if (req_pd_entries) {
    index = page_get_free_memory_index(req_pd_entries, req_pt_entries);
    if (index == ~0) { *err = NOT_ENOUGH_SPACE; return NULL; }
    return page_make_address(index, 0);
  }
  else {
    return page_get_free_pt_memory_index(req_pt_entries, err);
  }
}

/*
page_map maps a physical address to a desired virtual address
Input: Desired virtual address and entry flags, if no specific address is desired, parameter can be NULL
Length in pages, flags

Output: Mapped address, NULL if failed
*/
pgulong_t* page_map(pgulong_t* addr, size_t length, uint16_t flags) {

  int err = NO_ERROR;
  
  counter++;

  if (!addr) { addr = page_get_free_addr(length, &err); }
  if (err == NOT_ENOUGH_SPACE) { return NULL; }

  pgulong_t pd_index = pd_get_entry_index(addr);
  pgulong_t pt_index = page_get_entry_index(addr);

  pgulong_t* pt_addr = (((pgulong_t*)PD_ADDRESS)[pd_index] & 1) ? page_get_table_address(pd_index) 
    : pd_assign_table(pd_index, flags);

  for (unsigned int i = 0; i < length; i++, pt_index++) {

    if ((pgulong_t)pt_addr[pt_index] & 1) { return NULL; }   // If page table index was already mapped, fail
    
    pt_addr[pt_index] = (pgulong_t)palloc();
    pt_addr[pt_index] |= (flags & 0xFFF) | 1;

    flush_tlb_single(&pt_addr[pt_index]);
  }

  return addr;
}


/*
page_unmap unmaps a page
Input: Page frame's virtual address to unmap, length in pages
Output: True if succeeded, otherwise false
*/
bool page_unmap(pgulong_t* addr, size_t length) {

  if (!page_is_aligned(addr)) { return false; }        // Address should always be page aligned

  pgulong_t pd_index = pd_get_entry_index(addr);       // Page directory index
  pgulong_t pt_index = page_get_entry_index(addr);     // Page table index

  pgulong_t* pt_addr = page_get_table_address(pd_index);

  for (unsigned int i = 0; i < length; i++, pt_index++) {
    pt_addr[pt_index] = 0x0;                // Unmap entry
    flush_tlb_single(&pt_addr[pt_index]);   // Flush TLB to recognize changes
  }

  if (page_is_empty(pt_index)) { pd_remove_empty_pt(pd_index); }  // Remove entire page table from page directory if it's empty  
  
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
bool page_is_empty(pgulong_t pd_index) {

  if (!(((pgulong_t*)PD_ADDRESS)[pd_index] & 1)) { return true; }

  pgulong_t* addr = page_get_table_address(pd_index);

  for (uint16_t i = 0; i < ENTRIES; i++) {
    if (addr[i] & 1) { return false; }
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
Removes a page table from the page directory
Input: Page table's address to check, Page directory index to remove from
*/
void pd_remove_empty_pt(pgulong_t pd_index) {
  ((pgulong_t*)PD_ADDRESS)[pd_index] = 0x2;   // Set entry as not present
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
