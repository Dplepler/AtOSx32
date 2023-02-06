#ifndef HEAP_3_H
#define HEAP_3_H
#include "heap.h"

void init_heap_3();
void heap_split_header_3(heap_header_t* header);
void heap_remove_header_3(heap_header_t* header);
heap_header_t* heap_melt_left_3(heap_header_t* header);
void heap_insert_unused_header_3(heap_header_t* header);
void heap_eat_right_3(heap_header_t* header);
void free_3(void* ptr);
void* malloc_aligned(size_t size, uint32_t alignment);
void* calloc(size_t n, size_t size);
void* realloc(void* ptr, size_t size);
void* malloc(size_t size);
void free_aligned_3(void* ptr);
void* malloc_aligned(size_t size, uint32_t alignment);


#endif
