#include "FS.h"


uint8_t fat_buffer[SYSTEM_SECTORS - (HIDDEN_SECTORS + SECTORS_IN_FAT)];

void init_fat() {


  


}


uint16_t fat_extract_value(uint16_t index) {

  /* TODO: READ FROM DISK TO BUFFER */

   

  return (uint16_t)((uint16_t*)&fat_buffer[index]);
}
