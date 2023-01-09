#include "ata.h"

/* 400ns delay */
void io_delay() {

    inportb(PORT_ALT_STATUS_PRIMARY);
    inportb(PORT_ALT_STATUS_PRIMARY);
    inportb(PORT_ALT_STATUS_PRIMARY);
    inportb(PORT_ALT_STATUS_PRIMARY);
}

/* Read from specified address on the disk to the sector_buffer 
 * Input sector_index: LBA address in disk to read from
 * Input sectors: Amount of sectors to read
 * Input buffer: Buffer to read data to
 */
void ata_read(uint32_t sector_index, size_t sectors, void* buffer) {
  
  cli();

  while (inportb(PORT_COMMAND) & BSY) { } 
  
  /* Read one sector */
  outportb(PORT_SECTOR_COUNT, sectors);

  /* Choose starting sector */
  outportb(PORT_LBA_LOW,  (uint8_t)sector_index);          // First byte
  outportb(PORT_LBA_MID,  (uint8_t)(sector_index >> 8));   // Second byte
  outportb(PORT_LBA_HIGH, (uint8_t)(sector_index >> 16));  // Third byte
           
  /* Choose drive & head */
  outportb(PORT_DRIVE_HEAD, 0x40);
 
  outportb(PORT_COMMAND, ATA_READ);
 
  /* Wait until buffer is ready */
  while (inportb(PORT_COMMAND) & 0x80) { } 
  while (!(inportb(PORT_COMMAND) & 0x40)) { }  
  
  /* Read to buffer */
  for (uint16_t i = 0; i < sectors * (SECTOR_SIZE / 2); i++) {


    if (!(i % (SECTOR_SIZE / 2))) {
    
      /* Wait until buffer is ready: DRQ should be 1 and BSY should be 0 */
      while (inportb(PORT_COMMAND) & BSY) { } 
      while (!(inportb(PORT_COMMAND) & DRQ)) { }  
    }

    ((uint16_t*)buffer)[i] = inportw(PORT_DATA_PRIMARY);
    io_delay();
  }

  sti();
}

/* Write from the buffer to a specified addr on the disk
 * Input sector_index: LBA address to write to on the disk
 * Input sectors: Amount of sectors to write
 * Input buffer: Data to write to disk
 */
void ata_write(uint32_t sector_index, size_t sectors, void* buffer) {
  
  cli();

  while (inportb(PORT_COMMAND) & BSY) { } 
  
  /* Choose drive & head */
  outportb(PORT_DRIVE_HEAD, 0x40);
  
  outportb(PORT_SECTOR_COUNT, sectors);

  /* Choose starting sector */
  outportb(PORT_LBA_LOW,  (uint8_t)sector_index);          // First byte
  outportb(PORT_LBA_MID,  (uint8_t)(sector_index >> 8));   // Second byte
  outportb(PORT_LBA_HIGH, (uint8_t)(sector_index >> 16));  // Third byte

  outportb(PORT_COMMAND, ATA_WRITE);
 
  
  /* Write from buffer to disk */
  for (uint16_t i = 0; i < sectors * (SECTOR_SIZE / 2); i++) {
  
    if (!(i % (SECTOR_SIZE / 2))) {
    
      /* Wait until buffer is ready: DRQ should be 1 and BSY should be 0 */
      while (inportb(PORT_COMMAND) & BSY) { } 
      while (!(inportb(PORT_COMMAND) & DRQ)) { }  
    }

    outportw(PORT_DATA_PRIMARY, ((uint16_t*)buffer)[i]);
    io_delay();
  }

  sti();
}

