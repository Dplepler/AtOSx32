#include "heap.h"

heap_header*  free_pages[INIT_SIZE]     = { NULL };
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

  if (free_pages[header->index]) {
    header->flink = free_pages[header->index];
    free_pages[header->index]->blink = header;
  }

  free_pages[header->index] = header;
}

void heap_remove_header(heap_header* header) {

  if (free_pages[header->index] == header) { free_pages[header->index] = header->flink; }

  if (header->flink) { header->flink->blink = header->blink; }
  if (header->blink) { header->blink->flink = header->flink; }

  header->flink = NULL;
  header->blink = NULL;
}

heap_header* heap_allocate_header(unsigned int size) {

  size += sizeof(heap_header);

  unsigned int page_amount = heap_get_page_count(size);

  heap_header* header = (heap_header*)page_map(NULL, size, 0);

  header->signature = HEAP_SIGNATURE;
  header->size = page_amount * PAGE_SIZE;
  header->req_size = size - sizeof(heap_header);
  header->blink = NULL;
  header->flink = NULL;
  header->split_flink = NULL;
  header->split_blink = NULL;
  header->index = heap_get_index(header->size - sizeof(heap_header));

  return header;
}

void heap_split_header(heap_header* header) {

  if (1 << (header->size - (header->req_size + sizeof(heap_header))) < (1 << MIN_EXP)) { return; }   // Nothing to split

  heap_header* split_header = (heap_header*)((uint32_t)header + header->req_size + sizeof(heap_header));
  split_header->signature = HEAP_SIGNATURE;
  split_header->size = header->size - (header->req_size + sizeof(heap_header));   // Remainder size will now become the new header's size

  split_header->flink = header->split_flink;
  split_header->blink = header;

  if (header->split_flink) { header->split_flink->split_blink = split_header; }
  header->split_flink = split_header;

  header->size -= split_header->size;

  heap_insert_header(split_header);
}

heap_header* heap_melt_left(heap_header* header) {

  if (!header->split_blink) { return NULL; }

  heap_header* blink = header->split_blink;

  blink->size += header->size;
  blink->split_flink = header->split_flink;
  if (header->split_flink) { header->split_flink->split_blink = blink; }

  return blink;
}

void heap_eat_right(heap_header* header) {

  if (!header->split_flink) { return; }

  heap_header* flink = header->split_flink;

  heap_remove_header(flink);   // Header might be the first in the free pages index, we should remove it

  header->size += flink->size;
  if (flink->split_flink) { flink->split_flink->split_blink = header; }  

  flink = flink->split_flink;
}

void* malloc(size_t size) {

  unsigned int index = heap_get_index(size);
  //if (index < MIN_EXP) { index = MIN_EXP; }
  PRINTN(index);

  while(1) { }

  heap_header* header = free_pages[index];

  while (header) {
    if (header->size - sizeof(heap_header) >= size + sizeof(heap_header)) { break; }
    header = header->flink;
  }

  if (!header) { header = heap_allocate_header(size); }   // Get a new header
  else { 
    heap_remove_header(header);
    if (!header->split_flink && !header->split_blink) { complete_pages[header->index]--; }
  }

  heap_split_header(header); // If there's data that will never be used, split it to a new header

  return (void*)((uint32_t)header + sizeof(heap_header));
}

void free(void* ptr) {

  if (!ptr) { return; }   // Nothing to free

  heap_header* header = (heap_header*)((uint32_t)ptr - sizeof(heap_header));
  if (header->signature != HEAP_SIGNATURE) { return; }        // Not an allocated memory

  while (header->split_blink) { header = heap_melt_left(header); heap_remove_header(header); }
  while (header->split_flink) { heap_eat_right(header); }

  unsigned int index = heap_get_index(header->size - sizeof(heap_header));

  /* If we saved too much free unused pages we should free this one */
  if (complete_pages[index] == MAX_COMPLETE) { page_unmap((pgulong_t*)header, heap_get_page_count(header->size)); return; }

  complete_pages[index]++;
  
  heap_insert_header(header);
}

void* realloc(void* ptr, size_t size) {

  /* For special cases */
  if (!ptr) { return malloc(size); }
  if (!size) { free(ptr); return NULL; }

  heap_header* header = ((heap_header*)ptr - sizeof(heap_header));
  if (header->req_size == size) { return ptr; }

  /* Normal reallocation */
  void* np = malloc(size);
  
  memcpy(np, ptr, (header->req_size > size ? size : header->req_size));

  free(ptr);
  return np;
}

void* calloc(size_t n, size_t size) {

  void* ptr = malloc(n * size);
  memset(ptr, 0, n * size);

  return ptr;
}