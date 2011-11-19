#ifndef FAT_H__
#define  FAT_H__

#include <utils.h>
#include <kernutil.h>
#include <devices.h>
#include <file.h>
#include <error.h>

#define MAX_BLOCKS 100
#define BLOCK_SIZE 256

#define MAX_FILES 10
#define FILE_NAME_SIZE 10

/* End of Chain */
#define EOC -1

#define FREE_TYPE -1

/* Remember that this is an educational, memory allocated &
 single directory level FS developed to run easily on this
 ZeOS implementation, for a complete FS with i-nodes
 see https://github.com/bmiro/EmoFS */

char block_buffer [BLOCK_SIZE];

struct fat_dir_entry {
  char name[FILE_NAME_SIZE];
  int size;
  int block_count;
  int mode;
  int first_block;
  int last_block;
  int opens; /* Counts number of procs that opened the file */
  struct file_operations *fops;
};

struct data_block {
  char block [BLOCK_SIZE];
};

struct fat {
  /* Root directory pointing to files */
  struct fat_dir_entry root[MAX_FILES];
  
  int total_block_count;
  int used_block_count;
  int first_free_block;
  int last_free_block;
  int free_block_count;
  
  int block_lists[MAX_BLOCKS];
  
  struct data_block data[MAX_BLOCKS];
};

struct fat fs;

int initZeOSFAT();

/* Checks if path is in corrent format */
int check_path(const char *path);

/* Returns file identifier for the given path */
int find_path(const char *path);

/* Deletes a file from FAT metadata */
int delete_file(int file);

/* Creates a file in FAT metadata pre-allocating size bytes */
int fat_create(const char *path, int permissions,
	       struct file_operations *fops);
int fat_open(int file);
int fat_read(int f, char *buffer, int offset, int size);
int fat_write(int f, char *buffer, int offset, int size);


#endif
