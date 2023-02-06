#include "heap.h"

static heap_header_t* free_blocks[20] = { NULL };      // Each index represents 2^index allocated pages size
static uint8_t     complete_pages[20] = { 0 };         // Keep track of unused pages so we can free some if there are too many


void init_heap() {

  memset(free_blocks, 0, sizeof(heap_header_t*) * 20);
  memset(complete_pages, 0, 20);
}


/* Unused memory blocks will be indexed to the free_blocks array based on how many pages they take
   2^index <= pages < 2^(index+1) */
uint8_t heap_get_index(size_t size) {

  size_t pages = heap_get_page_count(size);
  
  if (pages > 0x100000) { return ~0; }   // More than 4GB of data is forbidden

  uint8_t index = 0;

  while ((uint8_t)(1 << index) <= pages) { index++; }
  return index - 1;
}

/* Insert memory block to the free blocks array */
void heap_insert_unused_header(heap_header_t* header) {

  header->index = heap_get_index(header->size - sizeof(heap_header_t));   // In case index is incorrect

  /* Insert header as the head node */
  if (free_blocks[header->index]) {
    free_blocks[header->index]->blink = header;
  }
  
  header->flink = free_blocks[header->index];
  free_blocks[header->index] = header; 
}

/* Remove memory block from the free blocks array */
void heap_remove_header(heap_header_t* header) {

  if (header->flink) { header->flink->blink = header->blink; }
  if (header->blink) { header->blink->flink = header->flink; }

  if (free_blocks[header->index] == header) { free_blocks[header->index] = header->flink; }

  header->flink = header->blink = NULL;
}

/* Split a header so we could later use it's remainder data 
   e.g if user malloced 10 bytes there would be a 4096(page) - 10 bytes of unused data,
   so we split it to a new header */
void heap_split_header(heap_header_t* header) {

  if (sizeof(heap_header_t) > header->size - header->req_size) { return; }
  if (sizeof(heap_header_t) > header->size - header->req_size - sizeof(heap_header_t)) { return; }
  
  heap_header_t* split_header = (heap_header_t*)((unsigned long)header + sizeof(heap_header_t) + header->req_size);
  
  split_header->size = split_header->req_size = header->size - header->req_size - sizeof(heap_header_t);

  split_header->req_size -= sizeof(heap_header_t);
  if (!split_header->size) { return; }

  split_header->signature = HEAP_SIGNATURE;

  if (header->page_flink) { header->page_flink->page_blink = split_header; }
  split_header->page_flink = header->page_flink;

  split_header->page_blink = header;
  header->page_flink = split_header;
  
  header->size -= split_header->size;

  split_header->used = false;
  split_header->flink = split_header->blink = NULL;
  
  heap_insert_unused_header(split_header);
}

/* Merge unused data from the same page together */
heap_header_t* heap_melt_left(heap_header_t* header) {

  if (header->used) { return header; }

  while (header->page_blink && !header->page_blink->used) {

    if (header->page_flink) { header->page_flink->page_blink = header->page_blink; }
  
    header->page_blink->page_flink = header->page_flink;
    header->page_blink->size += header->size;
     
    header = header->page_blink;
    heap_remove_header(header);
  }

  return header;
}

/* Absorb all the unused headers on the same page and to the right of the given header */
void heap_eat_right(heap_header_t* header) {

  if (header->used) { return; }

  while (header->page_flink && !header->page_flink->used) { 
    header->size += header->page_flink->size;
    if (header->page_flink->page_flink) { header->page_flink->page_flink->page_blink = header; }
    
    heap_remove_header(header->page_flink);
    header->page_flink = header->page_flink->page_flink;
  }
}

/* Create a new header for cases where there aren't any unused ones to use */
heap_header_t* heap_allocate_header(unsigned int size, uint16_t flags) {
  
  size_t page_amount = heap_get_page_count(size + sizeof(heap_header_t));  
  heap_header_t* header = (heap_header_t*)page_map(NULL, page_amount, flags);
  
  header->signature = HEAP_SIGNATURE;
  header->size = page_amount * PAGE_SIZE;
  header->req_size = size;

  header->flink = header->blink = header->page_flink = header->page_blink = NULL;

  header->index = heap_get_index(header->size - sizeof(heap_header_t));
  
  return header;
}

/* Allocate dynamic memory */
void* kmalloc(size_t size) {

  if (!size) { return NULL; }  

  uint8_t index = heap_get_index(size);
  heap_header_t* header = free_blocks[index];

  while (header) {
    if (header->size - sizeof(heap_header_t) >= size) { break; }
    header = header->flink;
  }

  if (!header) { header = heap_allocate_header(size, READ_WRITE); header->used = true; }   // Get a new header
  else {
    header->used = true;
    header->req_size = size;  
    heap_remove_header(header);   // Remove from free headers
    
    if (!header->page_flink && !header->page_blink) { complete_pages[header->index]--; }
  }
  
  heap_split_header(header);    // If there's extra space that will never be used, split it to a new header
  return (void*)((unsigned long)header + sizeof(heap_header_t));
}

/* Free allocated dynamic memory */
void free(void* ptr) {
  
  if (!ptr) { return; }   // Nothing to free
   
  heap_header_t* header = (heap_header_t*)((unsigned long)ptr - sizeof(heap_header_t));
  if (header->signature != HEAP_SIGNATURE) { return; }        // Not an allocated memory 
   
  uint8_t index = heap_get_index(header->size - sizeof(heap_header_t));
  header->used = false;

  /* Merge unused blocks located at the same page */ 
  header = heap_melt_left(header);
  heap_eat_right(header);  

  if (!header->page_flink && !header->page_blink) {
    
    /* If we saved too much free unused pages we should free this one */
    if (complete_pages[index] == MAX_COMPLETE) { page_unmap((pgulong_t*)header, heap_get_page_count(header->size)); return; }
    complete_pages[index]++;
  }

  heap_insert_unused_header(header);    
}

/* Resize an allocated memory block */
void* krealloc(void* ptr, size_t size) {

  /* For special cases */
  if (!ptr) { return kmalloc(size); }
  if (!size) { free(ptr); return NULL; }

  heap_header_t* header = (heap_header_t*)((unsigned long)ptr - sizeof(heap_header_t));
  if (header->req_size == size) { return ptr; }

  /* Normal reallocation */
  void* np = kmalloc(size);
  memcpy(np, ptr, (header->req_size > size ? size : header->req_size));

  free(ptr);
  return np;
}

/* Allocate and reset dynamic memory */
void* kcalloc(size_t n, size_t size) {

  void* ptr = kmalloc(n * size);
  memset(ptr, 0, n * size);

  return ptr;
}

/* Allocate aligned size, alignment type must be a power of 2 
Must use free_aligned in order to free the memory */
void* kmalloc_aligned(size_t size, uint32_t alignment) {

  if (!alignment || (alignment & (alignment - 1))) { return NULL; }  // Leave if alignment is not a power of 2
  
  /* 
  The way this works is we first allocate a size that would certainly fit,
  so we allocate our size + our alignment (starting the count from 0), 
  then, to make the pointer actually aligned, we first add our 
  alignment and then remove all the bits that exist after our alignment bit
  (because we only accept alignments that are a power of 2, we know how to remove 
  the right bits, that is the bits lower than the alignment bit), so we basically just
  floor the pointer to the closest aligned address
  */

  void* ptr = kmalloc(size + (alignment - 1));
  return (void*)(((uint32_t)ptr + (alignment - 1)) & ~(alignment - 1));
}

void free_aligned(void* ptr) {

  ptr = (void*)((uint32_t)ptr - sizeof(heap_header_t) + 0x4);
  
  while (((heap_header_t*)ptr)->signature != HEAP_SIGNATURE) { ptr = (void*)((uint32_t)ptr - 1); }
  
  free((void*)((unsigned long)ptr + sizeof(heap_header_t)));
}

