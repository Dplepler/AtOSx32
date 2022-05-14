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

  unsigned int index = heap_get_index(header->rsize - sizeof(heap_header));

  if (free_pages[index]) {
    header->flink = free_pages[index];
    free_pages[index]->blink = header;
  }

  free_pages[index] = header;
}


void* malloc(size_t size) {

  if (size < (1 << MIN_EXP)) { size = 1 << MIN_EXP; }  



}