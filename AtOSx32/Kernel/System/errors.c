#include "errors.h"

void panic(int error) {
  
  switch (error) {
    
    case NO_ERROR: return;

    case ERROR_NOT_ENOUGH_SPACE:      terminal_display_error("Not enough space in RAM :(\n\r");  return;
    case ERROR_NOT_ENOUGH_DISK_SPACE: terminal_display_error("Not enough space in disk :(\n\r"); return; 
  }





}
