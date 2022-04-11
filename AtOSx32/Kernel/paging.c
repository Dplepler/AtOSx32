#include "paging.h"

bool unmap_page(unsigned long* addr) {

    unsigned long pd_index = (unsigned long)addr >> 22;
    unsigned long pt_index = (unsigned long)addr >> 12 & 0x3FF;

    


}