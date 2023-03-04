#include "atoslib.h"

extern char getchar();
extern void* realloc(void* ptr, size_t size);
extern void* malloc(size_t size);
char* getline() {
  
  char c = 0;
  size_t size = 0;
  
  char* line = NULL;

  while ((c = getchar()) != '\n') { 
    line = realloc(line, ++size);
    line[size - 1] = c;
  }
  
  line = realloc(line, size + 1);
  line[size] = '\0';

  return line;
}

bool isdigit(char ch) {
  return ch >= '0' && ch <= '9';
}
