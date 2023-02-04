extern void print(char* str);
extern char getchar();

int main(void) {
  
  char a = getchar();
  
  char b[2];
  b[0] = a;
  b[1] = 0;

  print(b);

  while(1) {}
  return 0;
}

