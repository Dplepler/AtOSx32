
PORT_DRIVE_HEAD     equ 0x1F6
PORT_SECTOR_COUNT   equ 0x1F2

ATA_read:

  mov dx, DRIVE_HEAD_PORT
  mov al, 0xa0               ; Drive 0 & head 0
  
  mov dx, PORT_SECTOR_COUNT

