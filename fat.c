#include <fat.h>

int initZeOSFAT() {
  int i;
  
  for (i = 0; i < MAX_FILES; i++) {
    fs.root[i].type = FREE_TYPE;
    fs.root[i].size = 0;
    fs.root[i].first_block = EOC;
    fs.root[i].last_block = EOC;
    fs.root[i].file = EOC;
    fs.root[i].opens = 0;
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

/* Remember that spare files are not allowed, it will return EOC
 * if the block you requested is exactly one more that the 
 * file has. In other case it will return an error */
int fat_translate_block(struct fat_dir_entry *file, int logic_block) {
  int ph_block;
  int lb;
  
  ph_block = file->first_block;
  for (lb = 0; lb < logic_block; lb++) {
    if (ph_block == EOC) return -1;
    ph_block = fs.block_lists[ph_block];
  }
  
  /* Can return EOC, if caller is write a block must be allocated */
  return ph_block;
}

/* Adds a block to a file and returns a pointer to the ph_block */
int add_block_to_file(struct fat_dir_entry *file) {
  int new_block;
  
  /* Getting a free block */
  new_block = fs.first_free_block;
  /* Pointing to the new free block */
  fs.first_free_block = fs.block_lists[new_block];
  /* Assing free block to the file */
  fs.block_lists[file->last_block] = new_block;
  /* Mark the end of file*/
  file->last_block = new_block;
  fs.block_lists[last_block] = EOC;
  
}

int fat_read_block(struct data_block *block, int ph_block) {
  copy_data(fs.data[ph_block], block, BLOCK_SIZE);
  return 0;
}

int fat_write_block(struct data_block *block, int ph_block) {
  copy_data(block, fs.data[ph_block], BLOCK_SIZE);
  return 0;
}

int find_entry(struct fat_dir_entry *f, int file) {
  return &fs.root[file];
}

/***********************************************************************
/************** high level external public FAT functions ***************
/***********************************************************************/
/* Returns file identifier for the given path */
int find_path(const char *path);

/* Deletes a file from FAT metadata */
int delete_file(int file) {
  struct fat_dir_entry f;
  int b;
  
  find_entry(&f, file);
  
  if (f->opens != 0) return -1;
  
  /* Frees all assigned blocks putting them at the end
   * of free list */
  fs.block_lists[fs.last_free_block] = f.first_block;
  fs.free_block_count += (f.size % BLOCK_SIZE) + 1; 
  
  /* Frees dir entry */
  fs.root[file].mode = FREE_TYPE;
  fs.root[file].size = 0;
  fs.root[file].file = EOC;
}

/* Creates a file in FAT metadata pre-allocating size bytes */
int create_file(const char *path, int permissions) {
  int i;
  for (i = 0; i < MAX_FILES; i++) {
    if (fs.root[i].type == FREE_TYPE) {
      strcat(fs.root[i].name, path, "");
      fs.root[i].mode = (permissions & O_WRRD);
      fs.root[i].size = 0;
      fs.root[i].type = FILE_TYPE;
      return i;
    }
  }
  return -1;
}

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
  
  if (find_entry(&f, file) < 0) return -1;
  
  if (offset > f.size) return 0;

  error = find_entry(&f, file);
  if (error < 0) return error;
    
  first_block = offset / BLOCK_SIZE;
  block_offset = offset - (first_block * BLOCk_SIZE);
  block_count = ((offset + size) / BLOCK_SIZE) - first_block + 1;
  
  read = 0;
  remain = size;
  block_num = first_block;
  ph_block = fat_translate_block(&f, block_num);
  while (block_count) {
    if (ph_block == EOC) return read;
    
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
    
  /* Offset to write the first block */
  int block_offset;
  /* Ordinal number of firts block needed to write */
  int first_block;
  /* Number of blocks that will be affected by the write */
  int block_count;
  /* Number of blocks that the file must grow */
  int growing_blocks;
  int ph_block;
  
  if (find_entry(&f, file) < 0) return -1;

  /* We don't support sparse files */
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
    if (ph_block == EOC) { /* we need to grow! */
      ph_block = add_block_to_file(&f);
    }
    
    if (remain > BLOCK_SIZE) {
      block_remain = BLOCK_SIZE - block_offset;
    } else {
      block_remain = remain;
    }
    
    copy_data(buffer + written, &block_buffer + block_offset, block_remain);

    error = fat_write_block(&block_buffer, ph_block);
    if (error < 0) return error;
    
    if (block_offset != 0) block_offset = 0; /* Only needed the first time */
    
    f->size += block_remain;
    written += block_remain;
    remain -= block_remain;
    block_count--;
    block_num++;
    
    if (block_count > 0) {
      ph_block = fat_translate_block(&f, block_num);
    }
  }
}