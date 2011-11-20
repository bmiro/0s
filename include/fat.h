#ifndef FAT_H__
#define  FAT_H__

#include <utils.h>
#include <kernutil.h>
#include <devices.h>
#include <file.h>
#include <error.h>

#define MAX_BLOCKS 30
#define BLOCK_SIZE 256

#define MAX_FILES 10
#define FILE_NAME_SIZE 10

/* This define must be positive, else will be detected as an error */
#define FILE_NOT_FOUND (2^31)-1

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

/* Returns file identifier for the given path */
int fat_check_path(const char *path);
int fat_find_path(const char *path);

int fat_access_ok(int file, int flags);
int fat_is_in_use(int file);
int fat_create(const char *path, int flags, struct file_operations *fops);
int fat_open(int file);
int fat_close(int file);
int fat_read(int f, char *buffer, int offset, int count);
int fat_write(int f, const char *buffer, int offset, int count);
int fat_unlink(int file);

int fat_get_size(int file);
int fat_get_fops(int file, struct file_operations **fops);
int fat_get_opens(int file);

#endif
