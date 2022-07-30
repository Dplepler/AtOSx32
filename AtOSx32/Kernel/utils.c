#include "utils.h"

/* Get the character length of a string */
size_t strl(const char* str) {

  size_t len = 0;
  while (str[len]) { len++; }

  return len;
}

/* Get the digit length of an integer */
size_t intl(int n) {
  
  size_t length = 0;
  while (n > 0) { length++; n /= 10; }

  return length;
}

/*
itoa turns an integer into a char array
Input: Integer to convert, base to represent as (e.g 10 for decimal, 16 for hexadecimal..)
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

