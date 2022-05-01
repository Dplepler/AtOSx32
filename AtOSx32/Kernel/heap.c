#include "heap.h"

heap_header*  free_pages[INIT_SIZE];
uint8_t       used_pages[INIT_SIZE];

unsigned int heap_get_index(size_t size) {

  if (size < (1 << MIN_EXP)) { size = 1 << MIN_EXP; }

  unsigned int index = MIN_EXP;

  /* Find an index  */
  while (index < MAX_EXP) {
    if ((1 << index) > size) { break; }
    index++;
  }

  return index - 1;   // Get previous index, which is our desired one
}


void* malloc(size_t size) {

  



}