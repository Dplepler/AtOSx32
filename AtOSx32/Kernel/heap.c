#include "heap.h"

heap_header*  free_pages[INIT_SIZE] = { NULL };
uint8_t       used_pages[INIT_SIZE] = { 0 };

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

void heap_allocate_header(size_t size) {

  heap_header* header = (heap_header*)sbrk();


  


}

void* malloc(size_t size) {

  if (size < (1 << MIN_EXP)) { size = 1 << MIN_EXP; }  



}