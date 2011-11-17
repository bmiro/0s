#ifndef FAT_H__
#define  FAT_H__

#define MAX_BLOCKS 100
#define BLOCK_SIZE 256

#define MAX_FILES

/* Remember that this is an educational, memory allocated &
 single directory level FS developed to run easily on this
 ZeOS implementation, for a complete FS with i-nodes
 see https://github.com/bmiro/EmoFS */

struct data_block {
  char block [256];
};

struct fat {
  /* Root directory pointing to files */
  int root[MAX_FILES];
  
  int total_block_count;
  int used_block_count;
  int first_free_block;
  int free_block_count;
  
  int block_lists[MAX_BLOCKS];
  
  struct data_block data[MAX_BLOCKS];
};

struct fat fs;

int initZeOSFAT() {
  int i;
  
  for (i = 0; i < MAX_FILES; i++) {
    fs.root[i] = NULL;
  }
  
  fs.total_block_count = MAX_BLOCKS;
  fs.used_block_count = 0;
  fs.first_free_block = 0;
  fs.free_block_count = MAX_BLOCKS;
  
  for (i = 0; i < MAX_BLOCKS-1; i++) {
    fs.block_lists[i] = i+1;
  }
  fs.block_lists[i] = NULL;
}

/* Adds capatity to a file the number of size bytes */
int append_file_blocks(int file, int size);

/* Deletes a file from FAT metadata */
int delete_file(int file);

/* Creates a file in FAT metadata pre-allocating size bytes */
int create_file(int size, int permissions);



#endif
