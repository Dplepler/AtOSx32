#include "fs.h"


uint8_t fat_buffer[SECTORS_IN_FAT * SECTOR_SIZE];


void init_fat() {
    
  uint16_t num = fat_extract_value(0);
  if (num != 0xFFF8) { fat_setup_table(); }
  fat_setup_table();
}

void fat_setup_table() {
  
  memsetw(fat_buffer, EOC, SECTORS_IN_FAT * SECTOR_SIZE);
  ata_write(1, 1, fat_buffer, SECTORS_IN_FAT * SECTOR_SIZE);
}


uint16_t fat_extract_value(uint16_t index) {

  PRINTNH(SECTORS_IN_FAT); 
  //ata_read(HIDDEN_SECTORS, 100, fat_buffer, SECTORS_IN_FAT * SECTOR_SIZE); 
   
  return (uint16_t)(((uint16_t*)fat_buffer)[index]);
}


