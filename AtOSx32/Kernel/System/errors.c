#include "errors.h"

void panic(int error) {
  
  switch (error) {
    
    case NO_ERROR: return;

    case ERROR_NOT_ENOUGH_SPACE:      terminal_display_error("Not enough space in RAM :(\n\r");  break;
    case ERROR_NOT_ENOUGH_DISK_SPACE: terminal_display_error("Not enough space in disk :(\n\r"); break;
    case ERROR_FILE_TOO_LARGE:        terminal_display_error("File too big bruh\n\r");           break;
    case ERROR_PATH_INCORRECT:        terminal_display_error("Path is invalid\n\r");             break;
  }

  for(;;) {}



}
