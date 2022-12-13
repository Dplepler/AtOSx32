#ifndef ERRORS_H
#define ERRORS_H

#include "Drivers/kernel_screen.h"

typedef enum _ERRORS_ENUM {

  NO_ERROR = 0x0,

  ERROR_NOT_ENOUGH_SPACE,
  ERROR_NOT_ENOUGH_DISK_SPACE,
  ERROR_FILE_TOO_LARGE,
} error_t;


void panic(int error);

#endif
