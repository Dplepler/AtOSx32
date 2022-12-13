#include "fs.h"


uint8_t fat_buffer[FAT_BUFFER_SIZE];

void init_fat() {  
  if (fat_extract_value(0) != FAT_SIGNATURE) { fat_setup_table(); }
}

void fat_setup_table() {
 
  memsetw(fat_buffer, 0x0, FAT_BUFFER_SIZE);
  ((uint16_t*)fat_buffer)[0] = FAT_SIGNATURE;
  ata_write(HIDDEN_SECTORS + 1, SECTORS_IN_FAT, fat_buffer);
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

inode_t* create_file(char* filename, attribute_t attributes) {
  
  inode_t* inode = kcalloc(1, sizeof(inode_t));

  fat_create_filename(inode, filename);

  inode->attributes = attributes;
  
  cmos_time datetime = read_rtc();

  inode->creation_time = fat_create_time(datetime);
  inode->last_access_date = inode->creation_date = fat_create_date(datetime);

  return inode;
}

uint16_t fat_find_free_cluster(int* err) {
  
  READ_FAT(fat_buffer);

  for (uint16_t i = 0; i < (FAT_BUFFER_SIZE / sizeof(uint16_t)); i++) {
    if (VALID_CLUSTER(((uint16_t*)fat_buffer)[i])) {
      return ((uint16_t*)fat_buffer)[i];
    }
  }

  *err = ERROR_NOT_ENOUGH_DISK_SPACE;
  
  return 0;
}


void write_file(inode_t* inode, void* buffer, size_t size) {

  int err = NO_ERROR;
  
  inode->cluster = fat_find_free_cluster(&err);

  if (err) { panic(err); }



}
