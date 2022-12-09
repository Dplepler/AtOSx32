#include "ATA.h"

void ata_init() {




}


void ns400_delay() {

  inportb(PORT_ALT_STATUS);
  inportb(PORT_ALT_STATUS);
  inportb(PORT_ALT_STATUS);
  inportb(PORT_ALT_STATUS);
}


void lba28_select() {
  
  


}

void ata_read(uint32_t addr) {

  ouportb(PORT_COMMAND)

  outportb(PORT_DRIVE_HEAD, 0x40);  // TODO: Check we actually got the right drive

  




}
