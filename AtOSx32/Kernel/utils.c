#include "utils.h"

/*
strlen returns the length of a given char array
Input: Character array
Output: Length of character array
*/
size_t strlen(const char* str) {

	size_t len = 0;
	while (str[len]) { len++; }

	return len;
}

/*
itoa turns an integer into a char array
Input: Integer to convert, base to represent as (e.g 10 for decimal, 16 for hexadecimal..)
Output: A char array representing the value in the given base
*/
char* itoa(int value, size_t base) {

  static char buf[32] = { 0 };

  if (!value) { return "0"; }     // If value is 0, return a 0 string

  size_t i = 30;

  for (;value && i; i--, value /= base) {
    buf[i] = "0123456789abcdef"[value % base];
  }
  
  return &buf[i+1];
}