#include "utils.h"

void* memset(void* addr, uint8_t c, size_t n) {

  for (uint32_t i = 0; i < n; i++) {
    ((char*)addr)[i] = c;
  }

  return addr;
}

void* memsetw(void* addr, uint16_t val, size_t n) {
  
  for (uint32_t i = 0; i < n / 2; i++) {
    ((uint16_t*)addr)[i] = val;
  }

  return addr;
}

void memcpy(void* s1, const void* s2, size_t n) {
  for (; n > 0; n--) { *(uint8_t*)s1++ = *(uint8_t*)s2++; }
}

/* Check if a character is in the English alphabet */
bool isalpha(char c) {  
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) ? true : false;
}

/* Check if a character is a uppercase character */
bool isupper(char c) {
  return (c >= 'A' && c <= 'Z') ? true : false;
}

/* Check if a character is a lowercase character */
bool islower(char c) {
  return (c >= 'a' && c <= 'z') ? true : false;
}


/* Get the character length of a string */
size_t strl(const char* str) {
  
  if (!str) { return 0; }

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
  if (base > 16) { return NULL; } // Can't convert bases above hexadecimal

  size_t i = 30;

  for (; value && i; i--, value /= base) {
    buf[i] = "0123456789abcdef"[value % base];
  }
  
  return &buf[i+1];
}

/* Copy contents of one string into another */
char* strcpy(char* dest, const char* src) { 
  
  memcpy(dest, src, strl(dest));
  return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {

  memcpy(dest, src, n);
  return dest;
}

/* Copy src string to dest string */
char* strcat(char* dest, const char* src) {

  unsigned long i = 0;
  size_t dest_length = strl(dest);
  size_t src_length = strl(src);

  for(; i < dest_length; i++) {  }
  for (; i - dest_length < src_length; i++) { dest[i] = src[i - dest_length]; }  

  return dest;
}

/* Make all lower characters in the string uppercase */
char* upper(char* src, size_t n) {
  
  for (unsigned long i = 0; i < n; i++) { 
    if (islower(src[i])) { src[i] += ('A' - 'a'); }
  }

  return src;
}

int strcmp(const char* s1, const char* s2) {
  while(*s1 && (*s1 == *s2)) { s1++; s2++; }
  return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void strrev(char* str) {

  size_t size = strl(str);
  char c = 0;
  
  for (unsigned long i = 0; i < size / 2; i++) {
    c = str[i];
    str[i] = str[size - i - 1];
    str[size - i - 1] = c;
  }
}




