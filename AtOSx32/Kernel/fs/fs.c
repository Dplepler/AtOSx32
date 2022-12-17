#include "fs.h"

uint8_t* fat_buffer  = NULL;
uint8_t* root_buffer = NULL;

uint16_t root_entries = 0;

void init_fs() {

  fat_buffer = kcalloc(1, FAT_SIZE);
  root_buffer = kcalloc(1, ROOT_SIZE);

  if (fat_extract_value(0) != FAT_SIGNATURE) { 
    fat_setup_table();
    root_setup_dir();
  }
}


void root_setup_dir() {
  WRITE_ROOT(root_buffer);
}


void fat_setup_table() {
   
  ((uint16_t*)fat_buffer)[ROOT_SECTOR_OFFSET] = EOC;

  ((uint16_t*)fat_buffer)[0] = FAT_SIGNATURE;
  ((uint16_t*)fat_buffer)[1] = FAT_SIGNATURE;

  WRITE_FAT(fat_buffer);
}


uint16_t fat_extract_value(uint16_t index) {
  
  READ_FAT(fat_buffer);
  return (uint16_t)(((uint16_t*)fat_buffer)[index]);
}


void fat_create_filename(inode_t* inode, char* name) {

  char* it = name;

  while (*it != '.') { it++; }

  *it++ = '\0';

  memcpy(inode->filename, name, strl(name));
  memcpy(inode->ext, it, strl(it));
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

inode_t* create_directory(char* dirname, char* path, uint8_t attributes) {
  return create_file(dirname, path, attributes | ATTRIBUTE_DIRECTORY);
}

char* eat_path(char* path) {

  if (path == NULL) { return NULL; }

  char* it = path;

  while (!CHECK_SEPERATOR(*it)) {
    it++;
    if (((uint32_t)it - (uint32_t)path) > FULL_FILENAME_SIZE) { return NULL; }
  }

  size_t size = (uint32_t)it - (uint32_t)path;
  
  char* ret = kcalloc(1, size + 1);
  for (uint8_t i = 0; i < size; i++) { ret[i] = *path++; }

  return ret;
}


inode_t* navigate(char* path) {

  inode_t* parent = NULL;
  char* name = NULL;

  char* navigate_path = path;

  READ_ROOT(root_buffer);

  inode_t* current_file = NULL;
  char* buffer = (char*)root_buffer;

  while (navigate_path) {

    parent = current_file;
    
    name = eat_path(navigate_path);
    current_file = find_file(buffer, current_file->size, name);
    if (buffer != (char*)root_buffer) { free(buffer); }
    free(name);

    if (!(current_file->attributes & ATTRIBUTE_DIRECTORY)) { return parent; }

    buffer = read_file(current_file);
  }
  
  return current_file;
} 

char* make_full_filename(char* filename, char* ext) {

  char* filename_buff = kcalloc(1, strl(filename) + strl(ext) + sizeof(char) + 1);

  strcpy(filename_buff, filename);
  strcat(filename_buff, ".");
  strcat(filename_buff, ext);

  return filename_buff;
}


inode_t* find_file(char* buffer, size_t size, char* filename) {
  
  char* it = NULL;

  for (uint32_t i = 0; i < size; i += sizeof(inode_t)) {
    it = make_full_filename(((inode_t*)buffer)->filename, ((inode_t*)buffer)->ext);
    if (!strcmp(it, filename)) { free(it); return (inode_t*)buffer; }
    free(it);
  }

  panic(ERROR_PATH_INCORRECT);
  return NULL;
}


inode_t* create_file(char* filename, char* path, uint8_t attributes) {
    
  inode_t* file = init_file(filename, attributes);
  enter_file(file, navigate(path));
  return file;
}


inode_t* init_file(char* filename, uint8_t attributes) {
  
  inode_t* inode = kcalloc(1, sizeof(inode_t));

  fat_create_filename(inode, filename);

  inode->attributes = attributes;
  
  cmos_time datetime = read_rtc();

  inode->creation_time = fat_create_time(datetime);
  inode->last_access_date = inode->creation_date = fat_create_date(datetime);

  return inode;
}


void enter_file(inode_t* file, inode_t* dir) {
  cat_file(dir, file, sizeof(inode_t));
}


uint16_t fat_find_free_cluster(void* buffer, int* err) {
  
  for (uint16_t i = DATA_START; i < SYSTEM_SECTORS; i++) {
    if (!(((uint16_t*)buffer)[i])) { return i; }
  }

  *err = ERROR_NOT_ENOUGH_DISK_SPACE;
  
  return 0;
}

void write_file_data(inode_t* inode, void* buffer, size_t size) {

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

    buffer += CLUSTER_SIZE;
  }

  if (remainder) { 
   
    uint8_t* remainder_buffer = kmalloc(CLUSTER_SIZE);
    
    memcpy(remainder_buffer, buffer, remainder);
    memset((void*)((size_t)remainder_buffer + remainder), '\0', SECTOR_SIZE - remainder);

    ata_write(cluster * CLUSTERS_IN_SECTOR, CLUSTERS_IN_SECTOR, (void*)remainder_buffer);
    
    ((uint16_t*)fat_buffer)[cluster] = EOC;
    free(remainder_buffer);
  }

  inode->size = size;
  WRITE_FAT(fat_buffer);
}


void cat_file(inode_t* inode, void* buffer, size_t size) {

  uint8_t* file_data = read_file(inode);
  size_t prev_size = inode ? inode->size : (root_entries * DIR_ENTRY_SIZE);
  
  file_data = krealloc(file_data, prev_size + size);
  
  uint8_t* appended_buffer = (uint8_t*)((uint32_t)file_data + prev_size);
  memcpy(appended_buffer, buffer, size);
  
  fat_edit_file(inode, file_data, prev_size + size);
  free(file_data);
}


void fat_edit_file(inode_t* inode, void* buffer, size_t size) {
  
  if (!inode) { buffer = krealloc(buffer, ROOT_SIZE); WRITE_ROOT(buffer); return; }
  
  fat_delete_file(inode);
  write_file_data(inode, buffer, size);
}


void fat_delete_file(inode_t* inode) {
  
  READ_FAT(fat_buffer);
  
  uint16_t cluster = inode->cluster;
  uint16_t it = cluster;

  inode->cluster = 0x0;

  while (it != EOC) { it = fat_buffer[it]; fat_buffer[cluster] = 0x0; cluster = it; }

  fat_buffer[it] = 0x0;
  WRITE_FAT(fat_buffer);
}


void* read_file(inode_t* inode) {
  
  uint8_t* buffer = NULL;

  if (!inode) {
    
    READ_ROOT(root_buffer);
   
    buffer = kmalloc(root_entries * DIR_ENTRY_SIZE);
    memcpy(buffer, root_buffer, root_entries * DIR_ENTRY_SIZE);
    
    return buffer;
  }

  size_t buff_size = inode->size / SECTOR_SIZE;
  if (inode->size % SECTOR_SIZE) { buff_size += SECTOR_SIZE; }

  buffer = kmalloc(buff_size);
  READ_FAT(fat_buffer);

  uint8_t* it = buffer;
  
  for (uint16_t cluster = inode->cluster; cluster != EOC; cluster = ((uint16_t*)fat_buffer)[cluster]) {
    ata_read(cluster * CLUSTERS_IN_SECTOR, CLUSTERS_IN_SECTOR, it);
    it += CLUSTER_SIZE;
  }

  return buffer;
}

