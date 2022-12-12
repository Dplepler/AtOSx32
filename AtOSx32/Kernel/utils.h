#ifndef UTILS_H
#define UTILS_H
#include <stddef.h>
#include <stdbool.h>


char* itoa(int value, size_t base);
char* strcat(char* dest, const char* src);

size_t strl(const char* str);
size_t intl(int n);

bool isupper(char c);
bool islower(char c);

char* upper(char* src, size_t n);

#endif
