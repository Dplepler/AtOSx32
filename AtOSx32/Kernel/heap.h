#ifndef HEAP_H
#define HEAP_H

#include "vmm.h"

#define HEAP_SIGNATURE 0xB00B5555

#define INIT_SIZE 32

#define MIN_EXP 8
#define MAX_EXP 32

typedef struct _HEAP_HEADER_STRUCT {

  uint32_t signature;   // Signature magic
  size_t size;          // Requested size
  size_t rsize;         // Actual size created where rsize >= size

  unsigned int index;

  struct _HEAP_HEADER_STRUCT*  flink;
  struct _HEAP_HEADER_STRUCT*  blink;

  struct _HEAP_HEADER_STRUCT* split_flink;
  struct _HEAP_HEADER_STRUCT* split_blink;

} heap_header;

unsigned int heap_get_index(size_t size);
void* malloc(size_t size);



#endif