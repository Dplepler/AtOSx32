#include "ATA.h"

uint16_t buffer[BUFFER_SIZE] = { 0 };

void io_delay() {

    inportb(PORT_ALT_STATUS_PRIMARY);
    inportb(PORT_ALT_STATUS_PRIMARY);
    inportb(PORT_ALT_STATUS_PRIMARY);
    inportb(PORT_ALT_STATUS_PRIMARY);
}

void ata_read(uint32_t addr, uint8_t sector_count) {

  outportb(PORT_SECTOR_COUNT, sector_count);

  /* Choose address */
  outportb(PORT_LBA_LOW,  (uint8_t)addr);          // First byte
  outportb(PORT_LBA_MID,  (uint8_t)(addr >> 8));   // Second byte
  outportb(PORT_LBA_HIGH, (uint8_t)(addr >> 16));  // Third byte
           

  /* Choose drive & head */
  outportb(PORT_DRIVE_HEAD, 0x40);
 
  outportb(PORT_COMMAND, ATA_READ);
 
  /* Wait until buffer is ready */
  while (!(inportb(PORT_COMMAND) & BUFFER_READY)) { }  
  
  /* Read to buffer */
  for (uint16_t i = 0; i < 256; i++) { buffer[i] = inportw(PORT_DATA_PRIMARY); }
}

void ata_write(uint32_t addr, uint8_t sector_count) {

  outportb(PORT_SECTOR_COUNT, sector_count);

  /* Choose address */
  outportb(PORT_LBA_LOW,  (uint8_t)addr);          // First byte
  outportb(PORT_LBA_MID,  (uint8_t)(addr >> 8));   // Second byte
  outportb(PORT_LBA_HIGH, (uint8_t)(addr >> 16));  // Third byte
                                                

  /* Choose drive & head */
  outportb(PORT_DRIVE_HEAD, 0x40);
 
  outportb(PORT_COMMAND, 0x30);
 
  /* Wait until buffer is ready */
  while (!(inportb(PORT_COMMAND) & BUFFER_READY)) { }  
  
  uint16_t tmp = 0;

  /* Read from buffer to disk */
  for (uint16_t i = 0; i < 512; i++) {
    outportw(PORT_DATA_PRIMARY, buffer[i]);
    io_delay();
    //PRINT("");
    //outportb(PORT_COMMAND, 0xE7);
  } 

  
}

