extern void print(char* str);
extern char getchar();
extern char* getline();
extern void printh(void* ptr);


int main(void) { 

  char* hello = getline();
  
  print(hello);

  while(1) {} 
  return 0;
}

