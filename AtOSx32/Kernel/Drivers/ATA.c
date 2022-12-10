#include "ATA.h"

uint16_t sector_buffer[BUFFER_SIZE] = { 0 };

void io_delay() {

    inportb(PORT_ALT_STATUS_PRIMARY);
    inportb(PORT_ALT_STATUS_PRIMARY);
    inportb(PORT_ALT_STATUS_PRIMARY);
    inportb(PORT_ALT_STATUS_PRIMARY);
}

/* Read from specified address on the disk to the sector_buffer */
void ata_read_sector(uint32_t addr) {

  /* Read one sector */
  outportb(PORT_SECTOR_COUNT, 0x1);

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
  for (uint16_t i = 0; i < BUFFER_SIZE; i++) { sector_buffer[i] = inportw(PORT_DATA_PRIMARY); }

  io_delay();
}

/* Write from the sector_buffer to a specified addr on the disk */
void ata_write_sector(uint32_t addr) {

  /* Write one sector */
  outportb(PORT_SECTOR_COUNT, 0x1);

  /* Choose address */
  outportb(PORT_LBA_LOW,  (uint8_t)addr);          // First byte
  outportb(PORT_LBA_MID,  (uint8_t)(addr >> 8));   // Second byte
  outportb(PORT_LBA_HIGH, (uint8_t)(addr >> 16));  // Third byte
                                                

  /* Choose drive & head */
  outportb(PORT_DRIVE_HEAD, 0x40);
 
  outportb(PORT_COMMAND, 0x30);
 
  /* Wait until buffer is ready */
  while (!(inportb(PORT_COMMAND) & BUFFER_READY)) { }  
  
  /* Write from buffer to disk */
  for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
    outportw(PORT_DATA_PRIMARY, sector_buffer[i]);
    io_delay();
    outportw(PORT_DATA_PRIMARY, sector_buffer[i]);
    io_delay();
  }

}

