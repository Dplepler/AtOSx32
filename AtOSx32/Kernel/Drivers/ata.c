#include "ata.h"

void io_delay() {

    inportb(PORT_ALT_STATUS_PRIMARY);
    inportb(PORT_ALT_STATUS_PRIMARY);
    inportb(PORT_ALT_STATUS_PRIMARY);
    inportb(PORT_ALT_STATUS_PRIMARY);
}

/* Read from specified address on the disk to the sector_buffer 
 * Input: LBA address in disk to read from
 * Input: Size in sectors to read
 * Input: Buffer to read data to
 * Input: Size of specified buffer
 * */
void ata_read(uint32_t addr, uint16_t sector_size, void* buffer, size_t buffer_size) {

  /* Read one sector */
  outportb(PORT_SECTOR_COUNT, sector_size);

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
  for (uint16_t i = 0; i < buffer_size / 2; i++) { ((uint16_t*)buffer)[i] = inportw(PORT_DATA_PRIMARY); }

  io_delay();
}

/* Write from the buffer to a specified addr on the disk
 * Input: LBA address to write to on the disk
 * Input: Size in sectors to write 
 * Input: Buffer contains data to write to disk
 * Input: Size of specified buffer 
 */
void ata_write(uint32_t addr, uint16_t sector_count, void* buffer, size_t buffer_size) {

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
  
  /* Write from buffer to disk */
  for (uint16_t i = 0; i < buffer_size / 2; i++) {
    outportw(PORT_DATA_PRIMARY, ((uint16_t*)buffer)[i]);
    io_delay();
    outportw(PORT_DATA_PRIMARY, ((uint16_t*)buffer)[i]);
    io_delay();
  }
}

