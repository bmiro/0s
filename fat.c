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

/***********************************************************************
/************** Low level internal private FAT functions ***************
/***********************************************************************/
int fat_translate_block() {
  
}

int fat_read_block() {
  
}

int fat_write_block() {
  
}

/***********************************************************************
/************** high level external public FAT functions ***************
/***********************************************************************/
/* Returns dir entry for the given path */
int find_path(const char *path);

/* Adds capatity to a file the number of size bytes */
int append_file_blocks(int file, int size);

/* Deletes a file from FAT metadata */
int delete_file(int file);

/* Creates a file in FAT metadata pre-allocating size bytes */
int create_file(int size, int permissions);

int fat_read(int file, void *buffer, int offset, int size) {
  int read, remain;
  int error;
  
  struct dir_entry f;
  
  /* Offset to read the first block */
  int block_offset;
  /* Ordinal number of firts block needed to read */
  int first_block;
  /* Number of blocks that will be affected by the read */
  int block_count;
  
  if (offset > f.size) return 0;

  error = find_entry(&f, file);
  if (error < 0) return error;
    
  first_block = offset / BLOCK_SIZE;
  block_offset = offset - (first_block * BLOCk_SIZE);
  block_count = ((offset+size) / BLOCK_SIZE) - first_block + 1;
  
  read = 0;
  remain = size;
  block_num = first_block;
  ph_block = fat_translate_block(&f, block_num);
  while (block_count) {
    if (remain > BLOCK_SIZE) {
      block_remain = BLOCK_SIZE - block_offset;
    } else {
      block_remain = remain;
    }
    
    error = fat_read_block(&block_buffer, ph_block);
    if (error < 0) return error;
    
    copy_data(&block_buffer + block_offset, buffer, block_remain);
    
    if (block_offset != 0) block_offset = 0; /* Only needed the first time */
    
    read += block_remain;
    remain -= block_remain;
    block_count--;
    block_num++;
    
    if (block_count > 0) {
      ph_block = fat_translate_block(&f, block_num);
    }
  }
}

int fat_write(int file, void *buffer, int offset, int size) {
  int written, remain;
  int error;
  
  struct dir_entry f;
  
  /* Offset to write the first block */
  int block_offset;
  /* Ordinal number of firts block needed to write */
  int first_block;
  /* Number of blocks that will be affected by the write */
  int block_count;
  
  if (offset > f.size) return 0;

  error = find_entry(&f, file);
  if (error < 0) return error;
    
  first_block = offset / BLOCK_SIZE;
  block_offset = offset - (first_block * BLOCk_SIZE);
  block_count = ((offset + size) / BLOCK_SIZE) - first_block + 1;
  
  written = 0;
  remain = size;
  block_num = first_block;
  ph_block = fat_translate_block(&f, block_num);
  while (block_count) {
    if (remain > BLOCK_SIZE) {
      block_remain = BLOCK_SIZE - block_offset;
    } else {
      block_remain = remain;
    }
    
    copy_data(buffer + written, &block_buffer + block_offset, block_remain);

    error = fat_write_block(&block_buffer, ph_block);
    if (error < 0) return error;
    
    if (block_offset != 0) block_offset = 0; /* Only needed the first time */
    
    written += block_remain;
    remain -= block_remain;
    block_count--;
    block_num++;
    
    if (block_count > 0) {
      ph_block = fat_translate_block(&f, block_num);
    }
  }
}

