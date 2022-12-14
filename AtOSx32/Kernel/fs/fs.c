#include "fs.h"


uint8_t fat_buffer[FAT_BUFFER_SIZE];

void init_fat() {  
  if (fat_extract_value(0) != FAT_SIGNATURE) { fat_setup_table(); }
}

void fat_setup_table() {

  memsetw(fat_buffer, 0x0, FAT_BUFFER_SIZE);
  ((uint16_t*)fat_buffer)[0] = FAT_SIGNATURE;
  ((uint16_t*)fat_buffer)[1] = FAT_SIGNATURE;
  ata_write(HIDDEN_SECTORS, SECTORS_IN_FAT, fat_buffer);
}


uint16_t fat_extract_value(uint16_t index) {
  
  READ_FAT(fat_buffer);
  return (uint16_t)(((uint16_t*)fat_buffer)[index]);
}


void fat_create_filename(inode_t* inode, char* name) {

  char* it = name;

  while (*it != '.') { it++; }

  *it++ = '\0';

  strcpy(inode->filename, name);
  strncpy(inode->ext, it, EXTENTION_SIZE);
}

/* Convert time to FAT time format */
uint16_t fat_create_time(cmos_time time) {
  
  uint16_t fat_time = (time.seconds / 2);  // Seconds are counted in 2's 
  fat_time |= ((uint16_t)time.minute << 5);
  fat_time |= ((uint16_t)time.hour << 11);

  return fat_time;
}


uint16_t fat_create_date(cmos_time date) {
  
  uint16_t fat_date = date.day;
  fat_date |= ((uint16_t)date.month << 5);
  fat_date |= ((uint16_t)(date.year - 1980) <<  9);

  return fat_date;
}

inode_t* create_file(char* filename, unsigned int attributes) {
  
  inode_t* inode = kcalloc(1, sizeof(inode_t));

  fat_create_filename(inode, filename);

  inode->attributes = attributes;
  
  cmos_time datetime = read_rtc();

  inode->creation_time = fat_create_time(datetime);
  inode->last_access_date = inode->creation_date = fat_create_date(datetime);

  return inode;
}

uint16_t fat_find_free_cluster(void* buffer, int* err) {
  
  for (uint16_t i = DATA_START; i < SYSTEM_SECTORS; i++) {
    if (!(((uint16_t*)buffer)[i])) { return i; }
  }

  *err = ERROR_NOT_ENOUGH_DISK_SPACE;
  
  return 0;
}


void write_file(inode_t* inode, void* buffer, size_t size) {

  int err = NO_ERROR;

  READ_FAT(fat_buffer);
  
  size_t remainder = size % SECTOR_SIZE;
  size_t sectors = size / SECTOR_SIZE;

  if (sectors > SYSTEM_SECTORS) { err = ERROR_FILE_TOO_LARGE; return; }

  uint16_t cluster = inode->cluster = fat_find_free_cluster(fat_buffer, &err);
  if (err) { panic(err); }
 
  for (uint16_t i = 0; i < sectors; i++) {

    ata_write(cluster * CLUSTERS_IN_SECTOR, CLUSTERS_IN_SECTOR, buffer);   /* Write one cluster */ 
    
    ((uint16_t*)fat_buffer)[cluster] = EOC;
    if ((i != sectors - 1) || remainder) { ((uint16_t*)fat_buffer)[cluster] = fat_find_free_cluster(fat_buffer, &err); }
    if (err) { panic(err); }
 
    cluster = ((uint16_t*)fat_buffer)[cluster];

    buffer += SECTOR_SIZE;
  }

  if (remainder) { 

    uint8_t* remainder_buffer = kmalloc(SECTOR_SIZE);
    memcpy(remainder_buffer, buffer, remainder);
    memset((void*)((size_t)remainder_buffer + remainder), '\0', SECTOR_SIZE - remainder);

    ata_write(cluster * CLUSTERS_IN_SECTOR, CLUSTERS_IN_SECTOR, (void*)remainder_buffer);

    ((uint16_t*)fat_buffer)[cluster] = EOC;

    free(remainder_buffer);
  }

  inode->size = size;
  WRITE_FAT(fat_buffer);
}


void* read_file(inode_t* inode) {

  uint8_t* buffer = kmalloc(inode->size);

  READ_FAT(fat_buffer);

  uint16_t cluster = inode->cluster;
  uint8_t* it = buffer;
  
  for (uint16_t cluster = inode->cluster; cluster != EOC; cluster = ((uint16_t*)fat_buffer)[cluster]) {
      
    ata_read(cluster * CLUSTERS_IN_SECTOR, CLUSTERS_IN_SECTOR, it);
    it += SECTOR_SIZE;
  }

  return buffer;
}

