#ifndef PAGING_H
#define PAGING_H

#include <stdbool.h>
#include <stdint.h>

#define DIRECTORY_ADDR 0xC03FF000

bool unmap_page(unsigned long* addr);


#endif