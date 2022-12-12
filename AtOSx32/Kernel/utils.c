#include "utils.h"

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

