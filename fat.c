#include <fat.h>

int initZeOSFAT() {
  int i;
  
  for (i = 0; i < MAX_FILES; i++) {
    fs.root[i].mode = FREE_TYPE;
    fs.root[i].size = 0;
    fs.root[i].file = EOC;
  }
  
  fs.total_block_count = MAX_BLOCKS;
  fs.used_block_count = 0;
  fs.first_free_block = 0;
  fs.free_block_count = MAX_BLOCKS;
  
  for (i = 0; i < MAX_BLOCKS - 1; i++) {
    fs.block_lists[i] = i + 1;
  }
  fs.block_lists[i] = EOC;
}

/* Returns first data block for the given path */
int find_path(const char *path);

/* Adds capatity to a file the number of size bytes */
int append_file_blocks(int file, int size);

/* Deletes a file from FAT metadata */
int delete_file(int file);

/* Creates a file in FAT metadata pre-allocating size bytes */
int create_file(int size, int permissions);