#include "atoslib.h"

int a[1024];


void foo(int m[]) {

  m[10] = m[11];
  
}

int main(void) {
  
  foo(a);
  return a[10];
}
