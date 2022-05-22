#include "heap.h"

heap_header*  free_pages[INIT_SIZE] = { NULL };
bool          used_pages[INIT_SIZE] = { false };

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
  header->index = heap_get_index(header->size - sizeof(heap_header));

  return header;
}

void heap_split_header(heap_header* header) {

  if (heap_get_index(header->size - (header->req_size + sizeof(heap_header))) >= 0) { return; }   // Nothing to split

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


void* malloc(size_t size) {

  if (size < (1 << MIN_EXP)) { size = 1 << MIN_EXP; }  
  unsigned int index = heap_get_index(size);

  heap_header* header = free_pages[index];

  while (header) {
    if (header->size - sizeof(heap_header) >= size + sizeof(heap_header)) { break; }
    header = header->flink;
  }

  if (!header) { header = heap_allocate_header(size); }
  else { 
    heap_remove_header(header); 
    used_pages[index] = true;
  }

  heap_split_header(header); // If there's data that will never be used, split it to a new header

  return (void*)((uint32_t)header + sizeof(heap_header));
}