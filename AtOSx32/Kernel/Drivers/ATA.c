#include "ATA.h"

uint16_t buffer[SECTOR_SIZE / 2] = { 0 };


void ata_read(uint8_t sector_count, uint32_t addr) {

  outportb(PORT_SECTOR_COUNT, sector_count);

  /* Choose address */
  outportb(PORT_LBA_LOW,  (uint8_t)addr);          // First byte
  outportb(PORT_LBA_MID,  (uint8_t)(addr >> 8));   // Second byte
  outportb(PORT_LBA_HIGH, (uint8_t)(addr >> 16));  // Third byte
                                                

  /* Choose drive & head */
  //outportb(PORT_DRIVE_HEAD, 0x40);
  outportb(PORT_DRIVE_HEAD, 0x40);
 
  outportb(PORT_COMMAND, 0x20);

  while (!(inportb(PORT_COMMAND) & 0b1000)) { }   /* Wait until buffer is ready */
  
  
  for (uint16_t i = 0; i < (1 << 8); i++) { buffer[i] = inportw(PORT_DATA_PRIMARY); }

  PRINTNH(buffer[0]);
}

