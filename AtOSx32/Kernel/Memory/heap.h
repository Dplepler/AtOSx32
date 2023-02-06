#ifndef HEAP_H
#define HEAP_H

#include "Memory/vmm.h"

#define HEAP_SIGNATURE 0xB00B5555

#define INIT_SIZE 32

#define MIN_EXP 8
#define MAX_EXP 32

#define MAX_COMPLETE 5

typedef struct _HEAP_HEADER_STRUCT {

  uint32_t signature;    // Signature magic
  size_t req_size;       // Requested size
  size_t size;           // Actual size created where rsize >= size

  uint8_t index;
  bool used;

  struct _HEAP_HEADER_STRUCT*  flink;
  struct _HEAP_HEADER_STRUCT*  blink;

  struct _HEAP_HEADER_STRUCT* page_flink;
  struct _HEAP_HEADER_STRUCT* page_blink;

} heap_header_t;

static inline size_t heap_get_page_count(size_t length) {
  return (length / PAGE_SIZE) + ((length % PAGE_SIZE) ? 1 : 0);
} 

void* kmalloc(size_t size);
void* krealloc(void* ptr, size_t size);
void* kcalloc(size_t n, size_t size);
void* kmalloc_aligned(size_t size, uint32_t alignment);

void init_heap();
void free(void* ptr);
void free_aligned(void* ptr);
void heap_insert_unused_header(heap_header_t* header);
void heap_remove_header(heap_header_t* header);
void heap_split_header(heap_header_t* header);
void heap_eat_right(heap_header_t* header);

heap_header_t* heap_melt_left(heap_header_t* header);
heap_header_t* heap_allocate_header(unsigned int size, uint16_t flags);

uint8_t heap_get_index(size_t size);

#endif

