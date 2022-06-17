#include "heap.h"

heap_header*  free_headers[INIT_SIZE]   = { NULL };
unsigned int  complete_pages[INIT_SIZE] = { 0 };    // Count freed allocated pages

/*
Find an index based on the requested size
*/
unsigned int heap_get_index(size_t size) {

  if (size < (1 << MIN_EXP)) { return MIN_EXP; }

  size_t index = MIN_EXP;

  /* Find an index  */
  while (index < MAX_EXP) {
    if ((size_t)(1 << index) > size) { break; }
    index++;
  }

  return index - 1;   // Get previous index, which is our desired one
}

void heap_insert_header(heap_header* header) {

  header->index = heap_get_index(header->size - sizeof(heap_header));

  if (free_headers[header->index]) {
    header->flink = free_headers[header->index];
    free_headers[header->index]->blink = header;
  }

  free_headers[header->index] = header;
}

void heap_remove_header(heap_header* header) {

  if (free_headers[header->index] == header) { free_headers[header->index] = header->flink; }

  if (header->flink) { header->flink->blink = header->blink; }
  if (header->blink) { header->blink->flink = header->flink; }

  header->flink = NULL;
  header->blink = NULL;
}

heap_header* heap_allocate_header(unsigned int size) {

  size += sizeof(heap_header);

  unsigned int page_amount = heap_get_page_count(size);

  heap_header* header = (heap_header*)page_map(NULL, page_amount, 0);

  header->signature = HEAP_SIGNATURE;
  header->size = page_amount * PAGE_SIZE;
  header->req_size = size - sizeof(heap_header);
  header->blink = NULL;
  header->flink = NULL;
  header->split_flink = NULL;
  header->split_blink = NULL;
  header->index = heap_get_index(size);

  return header;
}

void heap_split_header(heap_header* header) {

  if (1 << (header->size - (header->req_size + sizeof(heap_header))) < (1 << MIN_EXP)) { return; }   // Nothing to split

  heap_header* split_header = (heap_header*)((uint32_t)header + header->req_size + sizeof(heap_header));
  
  split_header->signature = HEAP_SIGNATURE;
  split_header->size = header->size - (header->req_size + sizeof(heap_header));   // Remainder size will now become the new header's size
  split_header->split_flink = header->split_flink;
  split_header->split_blink = header;
  split_header->flink = split_header->blink = NULL;
  split_header->req_size = header->size - sizeof(heap_header);
 
  if (header->split_flink) { header->split_flink->split_blink = split_header; }
  header->split_flink = split_header;

  header->size -= split_header->size;
  heap_insert_header(split_header);
}

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
    heap_remove_header(header);
    if (!header->split_flink && !header->split_blink) { complete_pages[header->index]--; }
  }

  heap_split_header(header);    // If there's data that will never be used, split it to a new header

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