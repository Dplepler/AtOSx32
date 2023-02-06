#ifndef FS_H
#define FS_H

#include <stdint.h>
#include "Drivers/ata.h"
#include "Memory/heap3.h"
#include "Memory/heap.h"
#include "Drivers/kernel_screen.h"
#include "System/clock.h"
#include "System/errors.h"
 
#define HIDDEN_SECTORS 0x3
#define SECTOR_SIZE    0x200
#define CLUSTER_SIZE SECTOR_SIZE
#define CLUSTERS_IN_SECTOR (CLUSTER_SIZE / SECTOR_SIZE)
#define SYSTEM_SECTORS 0xFFFF
#define ROOT_ENTRIES 0X200
#define DIR_ENTRY_SIZE sizeof(inode_t)

#define SECTORS_IN_FAT ((SYSTEM_SECTORS * 2) / SECTOR_SIZE)
#define SECTORS_IN_ROOT ((ROOT_ENTRIES * DIR_ENTRY_SIZE) / SECTOR_SIZE)

#define KERNEL_SECTORS 100
#define KERNEL_CLUSTERS KERNEL_SECTORS
#define DATA_START (HIDDEN_SECTORS + SECTORS_IN_FAT + SECTORS_IN_ROOT + KERNEL_SECTORS)  /* Sector index indicating the beginning of data */

#define FAT_SIZE (SECTORS_IN_FAT * SECTOR_SIZE)
#define ROOT_SIZE (ROOT_ENTRIES * DIR_ENTRY_SIZE)
#define ROOT_CURRENT_SIZE (root_entries * DIR_ENTRY_SIZE)

#define FAT_SECTOR_OFFSET HIDDEN_SECTORS 
#define ROOT_SECTOR_OFFSET (HIDDEN_SECTORS + SECTORS_IN_FAT)

#define FAT_OFFSET (FAT_SECTOR_OFFSET * SECTOR_SIZE)
#define ROOT_OFFSET (ROOT_SECTOR_OFFSET * SECTOR_SIZE)

#define EOC         0xFFF8     /* End of cluster */
#define BAD_CLUSTER 0xFFF7

#define FAT_SIGNATURE 0xA00A

#define FILENAME_SIZE  0x8
#define EXTENTION_SIZE 0x3
#define FULL_FILENAME_SIZE (FILENAME_SIZE + EXTENTION_SIZE + sizeof(char))

#define get_next_cluster fat_extract_value

#define CHECK_SEPERATOR(c) (c == '/' || c == '\\')

#define VALID_CLUSTER(cluster) (cluster != BAD_CLUSTER && cluster > 0x2)


typedef enum ATTRIBUTES_ENUM {

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

  uint8_t attributes;

  uint8_t reserved1;

  uint8_t creation;
  
  uint16_t creation_time;
  uint16_t creation_date;
  uint16_t last_access_date;

  uint16_t reserved2;

  uint16_t last_write_time;
  uint16_t last_write_date;

  uint16_t cluster;
    
  uint32_t size;

} __attribute__((packed)) inode_t;


typedef struct _ELF_32_STRUCT {

        unsigned char   identify[16]; 
        uint16_t      type;
        uint16_t      machine;
        uint32_t      version;
        uint32_t      entry;
        uint32_t      phoff;
        uint32_t      shoff;
        uint32_t      flags;
        uint16_t      ehsize;
        uint16_t      phentsize;
        uint16_t      phnum;
        uint16_t      shentsize;
        uint16_t      shnum;
        uint16_t      shstrndx;

} elf32_header_t;

typedef struct _PROGRAM_HEADER_STRUCT {

  uint32_t type;
  uint32_t offset;
  uint32_t va;
  uint32_t pa;
  uint32_t filesz;
  uint32_t memsz;
  uint32_t flags; 
  uint32_t align;

} program_header_t;

uint32_t get_elf_size(elf32_header_t* elf);

void* read_file(inode_t* inode, bool kernel);
void* kread_file(inode_t* inode);
void* uread_file(inode_t* inode);

char* eat_path(char* path);
char* eat_path_reverse(char* path);
char* get_first_file_from_path(char* path);
char* get_last_file_from_path(char* path);
char* make_full_filename(char* filename, char* ext);

uint16_t fat_extract_value(uint16_t index);
uint16_t fat_create_time(cmos_time time);
uint16_t fat_create_date(cmos_time date);
uint16_t fat_find_free_cluster(void* buffer, int* err);

inode_t* create_file(char* filename, char* path, uint8_t attributes);
inode_t* create_directory(char* dirname, char* path, uint8_t attributes);
inode_t* navigate_dir(char* path, void** buff_ref);
inode_t* navigate_file(char* path, void** buff_ref);
inode_t* find_file(char* buffer, size_t size, char* filename);
inode_t* init_file(char* filename, uint8_t attributes);

void init_fs();
void load_shell();
void root_setup_dir();
void fat_setup_table();
void enter_file(inode_t* file, inode_t* dir);
void fat_resize_file(inode_t* inode, size_t size);
void init_first_cluster(inode_t* inode);
void cat_file(inode_t* inode, void* buffer, size_t size);
void edit_file(inode_t* inode, void* buffer, size_t size);
void fat_delete_file(inode_t* inode);
void fat_create_filename(inode_t* inode, char* name);
void write_file_data(inode_t* inode, void* buffer, size_t size);
void remove_dir_entry(char* dir_path, char* entry);

void write_file(char* path, void* buffer, size_t size);
void copy_file(char* old_path, char* new_path);
void move_file(char* old_path, char* new_path);
void delete_file(char* path);

bool file_has_extention(char* full_filename);


#endif

