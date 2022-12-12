#ifndef UTILS_H
#define UTILS_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void* memset(void* addr, uint8_t c, size_t n);
void* memsetw(void* addr, uint16_t val, size_t n);
void  memcpy(void* s1, const void* s2, size_t n);

char* itoa(int value, size_t base);
char* strcat(char* dest, const char* src);

size_t strl(const char* str);
size_t intl(int n);

bool isupper(char c);
bool islower(char c);

char* upper(char* src, size_t n);

char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);

#endif
