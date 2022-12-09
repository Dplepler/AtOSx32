#ifndef ATA_H 
#define ATA_H

#include "System/hal.h"

typedef enum ATA_PORTS_ENUM {


  PORT_DATA           = 0x1F0,
  PORT_DRIVE_HEAD     = 0x1F6,
  PORT_SECTOR_COUNT   = 0x1F2,
  PORT_SECTOR_NUMBER  = 0x1F3,
  PORT_CYLINDER_LOW   = 0x1F4,
  PORT_CYLINDER_HIGH  = 0x1F5,
  PORT_COMMAND        = 0x1F7,
  PORT_ALT_STATUS     = 0x3F6

} ata_port;


typedef struct _ATA_DEVICE_INFO_STRUCT {


  union {
    uint16_t lba_low;
    uint16_t sector_number;
  };

  union {
    uint16_t lba_mid;
    uint16_t cylinder_low;
  };

  union {
    uint16_t lba_high;
    uint16_t cylinder_high;
  };

  union {
    uint16_t head;
    uint16_t drive;
  };

  union {
    uint16_t command;
    uint16_t status;
  };

  union {
    uint16_t control;
    uint16_t status_alternative;
  };


} __attribute__((packed)) ata_device_t;










#endif
