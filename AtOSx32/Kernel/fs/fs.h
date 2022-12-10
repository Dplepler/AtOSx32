#ifndef FS_H
#define FS_H

#include <stdint.h>

#define HIDDEN_SECTORS 0x2
#define SECTOR_SIZE    0x200
#define CLUSTER_SIZE (SECTOR_SIZE * 8)
#define SYSTEM_SECTORS 0xFFFF
#define SECTORS_IN_FAT (SYSTEM_SECTORS * 2 / SECTOR_SIZE) 

#define FAT_OFFSET (HIDDEN_SECTORS * SECTOR_SIZE)

#define EOF_CLUSTER 0xFFF8
#define BAD_CLUSTER 0xFFF7

#define get_next_cluster fat_extract_value

void init_fat();


uint16_t fat_extract_value(uint16_t index);





#endif
