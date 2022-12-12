#ifndef FS_H
#define FS_H

#include <stdint.h>
#include "Drivers/ata.h"
#include "Memory/vmm.h"
#include "Drivers/kernel_screen.h"
#include "System/clock.h"

#define HIDDEN_SECTORS 0x2
#define SECTOR_SIZE    0x200
#define CLUSTER_SIZE (SECTOR_SIZE * 8)
#define SYSTEM_SECTORS 0xFFFF
#define SECTORS_IN_FAT ((SYSTEM_SECTORS * 2) / SECTOR_SIZE) 

#define FAT_OFFSET (HIDDEN_SECTORS * SECTOR_SIZE)

#define EOC        0xFFF8     /* End of cluster */
#define BAD_CLUSTER 0xFFF7

#define FAT_SIGNATURE 0xA00A

#define FILENAME_SIZE 8
#define EXTENTION_SIZE 3

#define get_next_cluster fat_extract_value

typedef enum _ATTRIBUTE_FLAGS_ENUM {

  ATTRIBUTE_READ = 0x0,
  ATTRIBUTE_HIDDEN,
  ATTRIBUTE_SYSTEM,
  ATTRIBUTE_VOLUME_LABEL,
  ATTRIBUTE_DIRECTORY,
  ATTRIBUTE_ACHIEVE

} attribute_t;


typedef struct _INODE_ENTRY_STRUCT {

  char filename[FILENAME_SIZE];
  char ext[EXTENTION_SIZE];

  attribute_t attributes;

  uint8_t reserved1;

  uint8_t creation;
  
  uint16_t creation_time;
  uint16_t creation_date;
  uint16_t last_access;

  uint16_t reserved2;

  uint16_t last_write_time;
  uint16_t last_write_date;

  uint16_t cluster;

  uint32_t size;

} __attribute__((packed)) inode_t;


void init_fat();
void fat_setup_table();
uint16_t fat_extract_value(uint16_t index);

void fat_create_filename(inode_t* inode, char* name);
uint16_t fat_create_time(cmos_time date);
uint16_t fat_create_date(cmos_time date);



#endif
