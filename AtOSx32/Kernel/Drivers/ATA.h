#ifndef ATA_H 
#define ATA_H

#include "System/hal.h"
#include <stdbool.h>
#include "kernel_screen.h"
#define ATA_READ  0x20
#define ATA_WRITE 0x30

#define BUFFER_SIZE  0xFF
#define BUFFER_READY 0b1000

typedef enum ATA_PORTS_ENUM {


  PORT_DATA_PRIMARY   = 0x1F0,
  PORT_FEATURES       = 0x1F1,
  PORT_SECTOR_COUNT   = 0x1F2,
  PORT_LBA_LOW        = 0x1F3,
  PORT_LBA_MID        = 0x1F4,
  PORT_LBA_HIGH       = 0x1F5,
  PORT_DRIVE_HEAD     = 0x1F6,
  PORT_COMMAND        = 0x1F7,
  
  PORT_DATA_SECONDARY = 0x170,
  
  PORT_ALT_STATUS_PRIMARY   = 0x3F6,
  PORT_ALT_STATUS_SECONDARY = 0x376,

} ata_port;



void ata_read(uint32_t addr, uint8_t sector_count);
void ata_write(uint32_t addr, uint8_t sector_count);
void io_delay();

#endif
