#ifndef LEXER_H
#define LEXER_H

typedef struct _LEXER_STRUCT {

  char* context;
  unsigned long index;
  char ch;

} lexer_t;

typedef struct _TOKEN_STRUCT {

  char* value;
  
  enum {
    
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    



  } type;
} token_t;



#endif

