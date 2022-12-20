#include "fs.h"

uint8_t* fat_buffer  = NULL;
uint8_t* root_buffer = NULL;

uint16_t root_entries = 0;

/* Initialize fat table and root directory if they weren't initialised yet */
void init_fs() {

  fat_buffer = kcalloc(1, FAT_SIZE);
  root_buffer = kcalloc(1, ROOT_SIZE);

  if (fat_extract_value(0) != FAT_SIGNATURE) { 
    fat_setup_table();
    root_setup_dir();
  }
}

/* Setup the root directory */
void root_setup_dir() {
  WRITE_ROOT(root_buffer);
}

/* Setup initial table values, indexes 0 & 1 are reserved */
void fat_setup_table() {
   
  ((uint16_t*)fat_buffer)[ROOT_SECTOR_OFFSET] = EOC;

  ((uint16_t*)fat_buffer)[0] = FAT_SIGNATURE;
  ((uint16_t*)fat_buffer)[1] = FAT_SIGNATURE;

  WRITE_FAT(fat_buffer);
}

/* Returns a value from the fat table at the given location */
uint16_t fat_extract_value(uint16_t index) {
  
  READ_FAT(fat_buffer);
  return (uint16_t)(((uint16_t*)fat_buffer)[index]);
}

/* Takes a file name and it's extention and creates a full filename string */
char* make_full_filename(char* filename, char* ext) {

  char* filename_buff = NULL;
  size_t size = 0;

  if (!ext || !*ext) { 
    
    size = strl(filename) + 1;
    filename_buff = kmalloc(size);
    memcpy(filename_buff, filename, size);

    return filename_buff; 
  }

  size = strl(filename) + strl(ext) + sizeof(char) + 1;

  filename_buff = kmalloc(size);

  memcpy(filename_buff, filename, strl(filename));
  strcat(filename_buff, ".");
  strcat(filename_buff, ext);

  filename_buff[size - 1] = '\0';
  
  return filename_buff;
}

/* Assign a node it's name */
void fat_create_filename(inode_t* inode, char* name) {

  char* it = name;
  while (*it != '.' && *it) { it++; }

  if (*it) { *it++ = '\0'; }
 
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

/* Convert date to FAT date format */
uint16_t fat_create_date(cmos_time date) {
  
  uint16_t fat_date = date.day;
  fat_date |= ((uint16_t)date.month << 5);
  fat_date |= ((uint16_t)(date.year - 1980) <<  9);

  return fat_date;
}

/* Creates a directory, for more information look at the create_file function */
inode_t* create_directory(char* dirname, char* path, uint8_t attributes) {
  return create_file(dirname, path, attributes | ATTRIBUTE_DIRECTORY);
}

/* Eats the first directory in a given path reference, deletes it from the string and returns it's name */
char* eat_path(char** path) {

  if (path == NULL) { return NULL; }

  char* it = *path;

  while (*it && !CHECK_SEPERATOR(*it)) {
    it++;
    if (((uint32_t)it - (uint32_t)*path) > FULL_FILENAME_SIZE) { return NULL; }
  }

  size_t size = (uint32_t)it - (uint32_t)*path;
  
  char* ret = kcalloc(1, size + 1);
  for (uint8_t i = 0; i < size; i++) { ret[i] = **path; (*path)++; }
  
  if (CHECK_SEPERATOR(*it)) { (*path)++; }
  return ret;
}

/* Takes a file path and returns the last specified directory */
inode_t* navigate_dir(char* path, inode_t* file, void** buff_ref) {
  
  if ((uint32_t)file - (uint32_t)root_buffer < ROOT_SIZE) { return NULL; }

  inode_t* parent = NULL;
  char* name = NULL;

  char* navigate_path = path;

  READ_ROOT(root_buffer);
  
  inode_t* current_file = NULL;
  char* buffer = (char*)root_buffer;

  while (navigate_path && *navigate_path) {
   
    parent = current_file;
    name = eat_path(&navigate_path);
   
    current_file = current_file ? find_file(buffer, current_file->size, name) : find_file(buffer, root_entries * DIR_ENTRY_SIZE, name);
    
    if (buffer != (char*)root_buffer) { free(buffer); }
 
    free(name);

    if (!(current_file->attributes & ATTRIBUTE_DIRECTORY)) { return parent; }
    if (file && file == current_file) { return parent; }
    
    if (navigate_path && *navigate_path) { buffer = read_file(current_file); }
    if (buff_ref) { *buff_ref = buffer; }
  }
  
  return current_file;
} 


/* Find a file by it's name from a given data buffer
 * The function searches for the file name in a directory data buffer
 * and returns the file node if it exists, or NULL, if it doesn't
 * */
inode_t* find_file(char* buffer, size_t size, char* filename) {
  
  char* it = NULL;
  
  for (uint32_t i = 0; i < size; i += DIR_ENTRY_SIZE, buffer += DIR_ENTRY_SIZE) {

    it = make_full_filename(((inode_t*)buffer)->filename, ((inode_t*)buffer)->ext);
    if (!strcmp(it, filename)) { free(it); return (inode_t*)buffer; } 
    free(it);
  }

  panic(ERROR_PATH_INCORRECT);
  return NULL;
}

/* Create a file 
 * Input: Desired filename, [up to 8 bytes of name].[up to 3 bytes of file extention]
 * Input: File path, where the file will be stored, this parameter can be null the directory is the root
 * Input: File attributes, more information in the header file
 * */
inode_t* create_file(char* filename, char* path, uint8_t attributes) {
    
  inode_t* file = init_file(filename, attributes); 
  
  void* dir_buffer;
  inode_t* dir = navigate_dir(path, NULL, &dir_buffer);
  
  enter_file(file, dir);
 
  if (dir) {
    
    inode_t* dir_dir = navigate_dir(path, dir, NULL);
    dir->size += DIR_ENTRY_SIZE;
    
    edit_file(dir_dir, dir_buffer, (dir_dir ? dir_dir->size : ROOT_SIZE));   /* Edit directory's size */
  }

  return file;
}

/* Initialize a file handler, with a specified name and file attributes */
inode_t* init_file(char* filename, uint8_t attributes) {
  
  inode_t* inode = kcalloc(1, sizeof(inode_t));
  
  fat_create_filename(inode, filename);

  inode->attributes = attributes;
  
  cmos_time datetime = read_rtc();

  inode->creation_time = fat_create_time(datetime);
  inode->last_access_date = inode->creation_date = fat_create_date(datetime);

  return inode;
}

/* Links the file to the specified directory */
void enter_file(inode_t* file, inode_t* dir) {
  cat_file(dir, file, DIR_ENTRY_SIZE);
}

/* Find first cluster that is marked as free in the fat table */
uint16_t fat_find_free_cluster(void* buffer, int* err) {
  
  for (uint16_t i = DATA_START; i < SYSTEM_SECTORS; i++) {
    if (!(((uint16_t*)buffer)[i])) { return i; }
  }

  *err = ERROR_NOT_ENOUGH_DISK_SPACE;
  
  return 0;
}


void fat_resize_file(inode_t* inode, size_t size) {

  uint16_t cluster = inode->cluster;
  uint16_t new_cluster_amount = size / CLUSTER_SIZE;
  if (size % CLUSTER_SIZE) { new_cluster_amount++; }

  uint16_t old_cluster_amount = inode->size / CLUSTER_SIZE;
  if (inode->size % CLUSTER_SIZE) { old_cluster_amount++; }


  READ_FAT(fat_buffer);

  if (inode->size > size) {

    uint16_t it = inode->cluster;

    for (uint16_t i = 0; i < new_cluster_amount; i++) { cluster = ((uint16_t*)fat_buffer)[cluster]; }
    it = ((uint16_t*)fat_buffer)[cluster];
    
    ((uint16_t*)fat_buffer)[cluster] = EOC;
    
    uint16_t it2 = it;
    while (it != EOC) { it = ((uint16_t*)fat_buffer)[it]; ((uint16_t*)fat_buffer)[it2] = 0x0; it2 = it; }

    goto write;
  }

  
  int err = NO_ERROR;
  
  if (!cluster) { 
    cluster = inode->cluster = fat_find_free_cluster(fat_buffer, &err); 
    if (err) { panic(err); } 

    ((uint16_t*)fat_buffer)[cluster] = EOC;
    inode->size = 0;
  }
  
  if (inode->size < size) {
    
    while (((uint16_t*)fat_buffer)[cluster] != EOC) { cluster = ((uint16_t*)fat_buffer)[cluster]; }
    
    size_t s = new_cluster_amount - old_cluster_amount - 1;
    
    for (uint16_t i = 0; i < s; i++) {
     
      ((uint16_t*)fat_buffer)[cluster] = fat_find_free_cluster(fat_buffer, &err); 
      if (err) { panic(err); }

      cluster = ((uint16_t*)fat_buffer)[cluster];
    }

    ((uint16_t*)fat_buffer)[cluster] = EOC;
  }

write:

  inode->size = size;
  WRITE_FAT(fat_buffer);
}


/* Write data to a specified file
 * Input: File node to link to created data
 * Input: Buffer containing the data to save (write)
 * Input: Size of buffer, how many bites to actually write
 * */
void write_file_data(inode_t* inode, void* buffer, size_t size) {

  int err = NO_ERROR;  

  size_t remainder = size % SECTOR_SIZE;
  size_t sectors = size / SECTOR_SIZE;

  if (sectors > SYSTEM_SECTORS) { err = ERROR_FILE_TOO_LARGE; return; }

  fat_resize_file(inode, size);
  
  READ_FAT(fat_buffer);

  uint16_t cluster = inode->cluster;

  do {

    ata_write(cluster * CLUSTERS_IN_SECTOR, CLUSTERS_IN_SECTOR, buffer);   /* Write one cluster */ 
    
    cluster = ((uint16_t*)fat_buffer)[cluster];
    buffer += CLUSTER_SIZE;

    size -= CLUSTER_SIZE;
  
  } while (cluster != EOC && size >= CLUSTER_SIZE);

  /* Last sector to write is smaller than the default sector size */
  if (remainder) { 
     
    uint8_t* remainder_buffer = kmalloc(CLUSTER_SIZE);
    
    memcpy(remainder_buffer, buffer, remainder);
    memset((void*)((size_t)remainder_buffer + remainder), '\0', SECTOR_SIZE - remainder);

    ata_write(cluster * CLUSTERS_IN_SECTOR, CLUSTERS_IN_SECTOR, (void*)remainder_buffer);
    
    free(remainder_buffer);
  }
}

/* Concatenate a file's contents with a given buffer and buffer size */
void cat_file(inode_t* inode, void* buffer, size_t size) {

  uint8_t* file_data = read_file(inode);
  size_t prev_size = inode ? inode->size : (root_entries++ * DIR_ENTRY_SIZE);

  file_data = krealloc(file_data, prev_size + size);

  uint8_t* appended_buffer = (uint8_t*)((uint32_t)file_data + prev_size);
  memcpy(appended_buffer, buffer, size);
  
  edit_file(inode, file_data, prev_size + size);

  free(file_data);  
}

/* Change a file's data to contain what's inside the given buffer, at the specified size */
void edit_file(inode_t* inode, void* buffer, size_t size) {
 
  /* Normal cases */
  if (inode) {
    write_file_data(inode, buffer, size);
    return;
  }

  /* Only for cases where the file is NULL, we're going to edit the root directory */
  void* root = kmalloc(ROOT_SIZE);
  memcpy(root, buffer, size);
  WRITE_ROOT(root);

  free(root);
}

/* Delete a specified file from the fat table & the fat table ONLY */
void fat_delete_file(inode_t* inode) {
  
  if (!inode || !inode->cluster) { return; }

  READ_FAT(fat_buffer);
  
  uint16_t cluster = inode->cluster;
  uint16_t it = cluster;

  inode->cluster = 0x0;

  while (it != EOC) { it = ((uint16_t*)fat_buffer)[it]; ((uint16_t*)fat_buffer)[cluster] = 0x0; cluster = it; }

  ((uint16_t*)fat_buffer)[it] = 0x0;
  WRITE_FAT(fat_buffer);
}

void fat_clear_file(inode_t* inode) {

  
  if (!inode || !inode->cluster) { return; }

  READ_FAT(fat_buffer);
  
  uint16_t cluster = inode->cluster;
  uint16_t it = cluster;

  while (it != EOC) { it = ((uint16_t*)fat_buffer)[it]; ((uint16_t*)fat_buffer)[cluster] = 0x0; cluster = it; }

  ((uint16_t*)fat_buffer)[it] = 0x0;
  ((uint16_t*)fat_buffer)[inode->cluster] = EOC;
  WRITE_FAT(fat_buffer);
}


/* Read and return a file's data */
void* read_file(inode_t* inode) {
  
  uint8_t* buffer = NULL;

  /* Read from root */
  if (!inode) {
   
    READ_ROOT(root_buffer);
    buffer = kmalloc(root_entries * DIR_ENTRY_SIZE);
    memcpy(buffer, root_buffer, root_entries * DIR_ENTRY_SIZE);
    
    return buffer;
  }

  /* Normal file read */
  size_t buff_size = inode->size / SECTOR_SIZE;
  if (inode->size % SECTOR_SIZE) { buff_size += SECTOR_SIZE; }
 
  if (!buff_size) { return NULL; }

  buffer = kmalloc(buff_size);
  READ_FAT(fat_buffer);

  uint8_t* it = buffer; 

  for (uint16_t cluster = inode->cluster; cluster != EOC; cluster = ((uint16_t*)fat_buffer)[cluster]) {
    ata_read(cluster * CLUSTERS_IN_SECTOR, CLUSTERS_IN_SECTOR, it);
    it += CLUSTER_SIZE;
  }

  return buffer;
}

