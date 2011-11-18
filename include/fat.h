#ifndef FAT_H__
#define  FAT_H__

#define MAX_BLOCKS 100
#define BLOCK_SIZE 256

#define MAX_FILES 10

/* End of Chain */
#define EOC -1

#define FREE_TYPE -1
#define FILE_TYPE 0
#define DIRECTORY_TYPE 1
#define DEVICE_TYPE 2


/* Remember that this is an educational, memory allocated &
 single directory level FS developed to run easily on this
 ZeOS implementation, for a complete FS with i-nodes
 see https://github.com/bmiro/EmoFS */

char block_buffer [BLOCK_SIZE];

struct dir_entry {
  char name[FILE_NAME_SIZE];
  int size;
  int data; /* Pointer to first data block */
  int mode;
  char type;
}

struct data_block {
  char block [BLOCK_SIZE];
};

struct fat {
  /* Root directory pointing to files */
  struct dir_entry root[MAX_FILES];
  
  int total_block_count;
  int used_block_count;
  int first_free_block;
  int free_block_count;
  
  int block_lists[MAX_BLOCKS];
  
  struct data_block data[MAX_BLOCKS];
};

struct fat fs;

int initZeOSFAT();

/* Returns first data block for the given path */
int find_path(const char *path);

/* Adds capatity to a file the number of size bytes */
int append_file_blocks(int file, int size);

/* Deletes a file from FAT metadata */
int delete_file(int file);

/* Creates a file in FAT metadata pre-allocating size bytes */
int create_file(int size, int permissions);

int fat_read(int file, void *buffer, int offset, int size);
int fat_write(int file, void *buffer, int offset, int size);


#endif
