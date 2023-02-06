#include "heap3.h"

static heap_header_t* free_blocks[20] = { NULL };
static uint8_t     complete_pages[20] = { 0 };

void init_heap_3() {
  memset(free_blocks, 0, sizeof(heap_header_t*) * 20);
  memset(complete_pages, 0, 20);
}

void heap_split_header_3(heap_header_t* header) {

  if (sizeof(heap_header_t) > header->size - header->req_size) { return; }
  if (sizeof(heap_header_t) > header->size - header->req_size - sizeof(heap_header_t)) { return; }
  
  heap_header_t* split_header = (heap_header_t*)((unsigned long)header + sizeof(heap_header_t) + header->req_size);
  
  split_header->size = split_header->req_size = header->size - header->req_size - sizeof(heap_header_t);

  split_header->req_size -= sizeof(heap_header_t);
  if (!split_header->size) { return; }

  split_header->signature = HEAP_SIGNATURE;

  if (header->page_flink) { header->page_flink->page_blink = split_header; }
  split_header->page_flink = header->page_flink;

  split_header->page_blink = header;
  header->page_flink = split_header;
  
  header->size -= split_header->size;

  split_header->used = false;
  split_header->flink = split_header->blink = NULL;
  
  heap_insert_unused_header_3(split_header);
}

void heap_remove_header_3(heap_header_t* header) {

  if (header->flink) { header->flink->blink = header->blink; }
  if (header->blink) { header->blink->flink = header->flink; }

  if (free_blocks[header->index] == header) { free_blocks[header->index] = header->flink; }

  header->flink = header->blink = NULL;
}

heap_header_t* heap_melt_left_3(heap_header_t* header) {

  if (header->used) { return header; }

  while (header->page_blink && !header->page_blink->used) {

    if (header->page_flink) { header->page_flink->page_blink = header->page_blink; }
  
    header->page_blink->page_flink = header->page_flink;
    header->page_blink->size += header->size;
     
    header = header->page_blink;
    heap_remove_header_3(header);
  }

  return header;
}

void heap_insert_unused_header_3(heap_header_t* header) {

  header->index = heap_get_index(header->size - sizeof(heap_header_t));   // In case index is incorrect

  /* Insert header as the head node */
  if (free_blocks[header->index]) {
    free_blocks[header->index]->blink = header;
  }
  
  header->flink = free_blocks[header->index];
  free_blocks[header->index] = header; 
}

void heap_eat_right_3(heap_header_t* header) {

  if (header->used) { return; }

  while (header->page_flink && !header->page_flink->used) { 
    header->size += header->page_flink->size;
    if (header->page_flink->page_flink) { header->page_flink->page_flink->page_blink = header; }
    
    heap_remove_header_3(header->page_flink);
    header->page_flink = header->page_flink->page_flink;
  }
}

void free_3(void* ptr) {
  
  if (!ptr) { return; }   // Nothing to free
   
  heap_header_t* header = (heap_header_t*)((unsigned long)ptr - sizeof(heap_header_t));
  if (header->signature != HEAP_SIGNATURE) { return; }        // Not an allocated memory 
   
  uint8_t index = heap_get_index(header->size - sizeof(heap_header_t));
  header->used = false;

  /* Merge unused blocks located at the same page */ 
  header = heap_melt_left_3(header);
  heap_eat_right_3(header);  

  if (!header->page_flink && !header->page_blink) {
    
    /* If we saved too much free unused pages we should free this one */
    if (complete_pages[index] == MAX_COMPLETE) { page_unmap((pgulong_t*)header, heap_get_page_count(header->size)); return; }
    complete_pages[index]++;
  }

  heap_insert_unused_header_3(header);    
}

void free_aligned_3(void* ptr) {

  ptr = (void*)((uint32_t)ptr - sizeof(heap_header_t) + 0x4);
  
  while (((heap_header_t*)ptr)->signature != HEAP_SIGNATURE) { ptr = (void*)((uint32_t)ptr - 1); }
  
  free((void*)((unsigned long)ptr + sizeof(heap_header_t)));
}

void* malloc(size_t size) {

  if (!size) { return NULL; }  

  uint8_t index = heap_get_index(size);
  heap_header_t* header = free_blocks[index];

  while (header) {
    if (header->size - sizeof(heap_header_t) >= size) { break; }
    header = header->flink;
  }

  if (!header) { header = heap_allocate_header(size, USER_ACCESS); header->used = true; }   // Get a new header
  else {
    header->used = true;
    header->req_size = size;  
    heap_remove_header_3(header);   // Remove from free headers
    
    if (!header->page_flink && !header->page_blink) { complete_pages[header->index]--; }
  }
  
  heap_split_header_3(header);    // If there's extra space that will never be used, split it to a new header
                                  
  return (void*)((unsigned long)header + sizeof(heap_header_t));
}


/* Resize an allocated memory block */
void* realloc(void* ptr, size_t size) {

  /* For special cases */
  if (!ptr) { return malloc(size); }
  if (!size) { free(ptr); return NULL; }

  heap_header_t* header = (heap_header_t*)((unsigned long)ptr - sizeof(heap_header_t));
  if (header->req_size == size) { return ptr; }

  /* Normal reallocation */
  void* np = malloc(size);
  memcpy(np, ptr, (header->req_size > size ? size : header->req_size));

  free(ptr);

  return np;
}

/* Allocate and reset dynamic memory */
void* calloc(size_t n, size_t size) {

  void* ptr = malloc(n * size);
  memset(ptr, 0, n * size);

  return ptr;
}

void* malloc_aligned(size_t size, uint32_t alignment) {

  if (!alignment || (alignment & (alignment - 1))) { return NULL; }  // Leave if alignment is not a power of 2

  void* ptr = malloc(size + (alignment - 1));
  return (void*)(((uint32_t)ptr + (alignment - 1)) & ~(alignment - 1));
}

