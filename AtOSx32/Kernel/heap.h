#ifndef HEAP_H
#define HEAP_H

#include "vmm.h"

#define HEAP_SIGNATURE 0xB00B5555

#define INIT_SIZE 32

#define MIN_EXP 8
#define MAX_EXP 32

#define MAX_COMPLETE 5

typedef struct _HEAP_HEADER_STRUCT {

  uint32_t signature;    // Signature magic
  size_t req_size;       // Requested size
  size_t size;           // Actual size created where rsize >= size

  unsigned int index;

  struct _HEAP_HEADER_STRUCT*  flink;
  struct _HEAP_HEADER_STRUCT*  blink;

  struct _HEAP_HEADER_STRUCT* split_flink;
  struct _HEAP_HEADER_STRUCT* split_blink;

} heap_header;

static inline size_t heap_get_page_count(size_t length) {
  return (length / PAGE_SIZE) + ((length % PAGE_SIZE) ? 1 : 0);
} 

void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void* calloc(size_t n, size_t size);
void free(void* ptr);
unsigned int heap_get_index(size_t size);
void heap_insert_header(heap_header* header);
void heap_remove_header(heap_header* header);
void heap_split_header(heap_header* header);
heap_header* heap_melt_left(heap_header* header);
void heap_eat_right(heap_header* header);
heap_header* heap_allocate_header(unsigned int size);



#endif