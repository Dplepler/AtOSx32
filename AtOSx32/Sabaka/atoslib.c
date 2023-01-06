#include "atoslib.h"

void print(const char* str) {
  
  __asm__ __volatile__ ( " \
    mov $1, %%ah;  \
    mov %0, %%esi; \
    int $0x45; "
    : : "r"((unsigned long)str));
}

