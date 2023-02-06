#include "atoslib.h"

extern char getchar();
extern void* realloc(void* ptr, size_t size);
extern void* malloc(size_t size);
char* getline() {
  
  //char c = 0;
  //size_t size = 0;
  /*while ((c = getchar()) != '\n') { 
    line = realloc(line, ++size);
    line[size - 1] = c;
  }
  
  line = realloc(line, size + 1);
  line[size] = '\0'; */

  int x = 0;

  char* line = NULL;
  while (x < 1) {
    line = realloc(line, 1);
    line[0] = 'a';
    x++;
  }

  //line = realloc(line, 2);
  line[1] = 0;

  return line;
}
