#include "heap.h"

heap_header*  free_pages[INIT_SIZE] = { NULL };
uint8_t       used_pages[INIT_SIZE] = { 0 };

/*
Find an index based on the requested size
*/
unsigned int heap_get_index(size_t size) {

  if (size < (1 << MIN_EXP)) { size = 1 << MIN_EXP; }

  size_t index = MIN_EXP;

  /* Find an index  */
  while (index < MAX_EXP) {
    if ((size_t)(1 << index) > size) { break; }
    index++;
  }

  return index - 1;   // Get previous index, which is our desired one
}

void insert_header(heap_header* header) {

  header->index = heap_get_index(header->size - sizeof(heap_header));

  if (free_pages[header->index]) {
    header->flink = free_pages[header->index];
    free_pages[header->index]->blink = header;
  }

  free_pages[header->index] = header;
}

void remove_header(heap_header* header) {

  if (free_pages[header->index] == header) { free_pages[header->index] = header->flink; }

  if (header->flink) { header->flink->blink = header->blink; }
  if (header->blink) { header->blink->flink = header->flink; }

  header->flink = NULL;
  header->blink = NULL;
}

heap_header* allocate_header(unsigned int size) {

  size += sizeof(heap_header);

  unsigned int page_amount = size / PAGE_SIZE;

  if (size % PAGE_SIZE) { ++page_amount; }

  heap_header* header = (heap_header*)sbrk(page_amount);

  header->signature = HEAP_SIGNATURE;
  header->size = page_amount * PAGE_SIZE;
  header->req_size = size - sizeof(heap_header);
  header->blink = NULL;
  header->flink = NULL;
  header->split_flink = NULL;
  header->split_blink = NULL;

  return header;
}


void* malloc(size_t size) {

  if (size < (1 << MIN_EXP)) { size = 1 << MIN_EXP; }  
  heap_header* header = free_pages[heap_get_index(size)];

  while (header) {
    if (header->size - sizeof(heap_header) >= size + sizeof(heap_header)) { break; }
    header = header->flink;
  }

  if (!header) { header = allocate_header(size); }
}