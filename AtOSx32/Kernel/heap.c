#include "heap.h"

static heap_header free_blocks[32]    // Each index represents 2^index allocated pages size

unsigned int heap_get_index(size_t size) {

  size_t pages = heap_get_page_count(size);
  
  if (pages > 0x100000) { return; }   // More than 4GB of data is forbidden

  unsigned int index = 0;

  while (1 << index > pages) { index++; }
  return index - 1;
}


void heap_insert_unused_header(heap_header* header) {

  header->index = heap_get_index(header->size);   // In case index is incorrect
  
  /* Insert header as the head node */
  if (free_blocks[header->index]) { 
    free_blocks[header->index]->blink = header; 
    header->flink = free_blocks[header->index];
  }

  free_blocks[header->index] = header;
}

void heap_remove_header(heap_header* header) {

  if (header->flink) { header->flink->blink = header->blink; }

  if (free_blocks[header->index] == header) { free_blocks[header->index] = header->flink; }
  else if (header->blink) { header->blink->flink = header->flink; }

  header->flink = NULL;
  header->blink = NULL;
}

void heap_split_header(heap_header* header) {

  

}


heap_header* heap_melt_left(heap_header* header);
void heap_eat_right(heap_header* header);
heap_header* heap_allocate_header(unsigned int size);


/* Allocate dynamic memory */
void* malloc(size_t size) {

  unsigned int index = heap_get_index(size);

  heap_header* header = free_headers[index];
  
  while (header) {
    if (header->size - sizeof(heap_header) >= size) { break; }
    header = header->flink;
  }
  
  if (!header) { header = heap_allocate_header(size); }   // Get a new header
  else { 
    heap_remove_header(header);   // Remove from free headers
    if (!header->split_flink && !header->split_blink) { complete_pages[header->index]--; }
  }

  heap_split_header(header);    // If there's extra space that will never be used, split it to a new header

  return (void*)((uint32_t)header + sizeof(heap_header));
}

/* Free allocated dynamic memory */
void free(void* ptr) {

  if (!ptr) { return; }   // Nothing to free

  heap_header* header = (heap_header*)((uint32_t)ptr - sizeof(heap_header));
  if (header->signature != HEAP_SIGNATURE) { return; }        // Not an allocated memory 

  unsigned int index = heap_get_index(header->size - sizeof(heap_header));
  
  if (!header->split_flink && !header->split_blink) {
    /* If we saved too much free unused pages we should free this one */
    if (complete_pages[index] == MAX_COMPLETE) { page_unmap((pgulong_t*)header, heap_get_page_count(header->size)); return; }
    complete_pages[index]++;
  }

  heap_insert_header(header);
}

/* Resize an allocated memory block */
void* realloc(void* ptr, size_t size) {

  /* For special cases */
  if (!ptr) { return malloc(size); }
  if (!size) { free(ptr); return NULL; }

  heap_header* header = (heap_header*)((unsigned long)ptr - sizeof(heap_header));
  if (header->req_size == size) { return ptr; }

  /* Normal reallocation */
  void* np = malloc(size);
  
  memcpy(np, ptr, (header->req_size > size ? size : header->req_size));

  free(ptr);
  return np;
}

/* Allocate and reset dynamic memory */
void* calloc(size_t n, size_t size) {

  void* ptr = malloc(n * size);
  memset(ptr, 0, n * size);

  return ptr;
}
