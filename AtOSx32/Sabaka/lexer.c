#include "lexer.h"

token_t* init_token(int type, char* value) {}


lexer_t* init_lexer() {
  
  lexer_t* lexer = malloc(sizeof(lexer));

  lexer->index = 0; 
  lexer->context = NULL;
  lexer->ch = 0;

  return lexer;
}

void lexer_advance(lexer_t* lexer) {
  lexer->ch = lexer->ch != '\n' ? lexer->context[++lexer->index] : '\0'; 
}

void lexer_skip_whitespace(lexer_t* lexer) {
  
  while (lexer->ch == ' ' || lexer->ch == '\r' || lexer->ch == '\t') {
    lexer_advance(lexer);     
    
  }
}


token_t* lexer_get_token(lexer_t* lexer) {
  
  lexer_skip_whitespace(lexer);

  if (isdigit(lexer->ch)) { lexer_collect_number(lexer); }


  

}

token_T* lexer_collect_number(lexer_t* lexer) {

  char* num = NULL;
  size_t size = 0;

  /* Collect number */
  while (isdigit(lexer->ch)) {
    num = realloc(num, ++size);
    num[size - 1] = lexer->ch;
    lexer_advance(lexer);
  }

  /* Terminate string with a 0 */
  num = realloc(num, size + 1);
  num[size] = '\0';

  return init_token(TOKEN_NUMBER, num);
}
