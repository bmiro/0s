#include <fat.h>

int initZeOSFAT() {
  int i;
  
  for (i = 0; i < MAX_FILES; i++) {
    strcat(fs.root[i].name, "", "");
    fs.root[i].mode = FREE_TYPE;
    fs.root[i].size = 0;
    fs.root[i].first_block = EOC;
    fs.root[i].last_block = EOC;
    fs.root[i].opens = 0;
  }
  
  fs.total_block_count = MAX_BLOCKS;
  fs.used_block_count = 0;
  fs.first_free_block = 0;
  fs.last_free_block = MAX_BLOCKS-1;
  fs.free_block_count = MAX_BLOCKS;

  
  for (i = 0; i < MAX_BLOCKS - 1; i++) {
    fs.block_lists[i] = i + 1;
  }
  fs.block_lists[i] = EOC;
}

/***********************************************************************
 ************** Low level internal private FAT functions ***************
 ***********************************************************************/

/* Remember that spare files are not allowed, it will return EOC
 * if the block you requested is exactly one more that the 
 * file has. In other case it will return an error */
int fat_translate_block(int file, int logic_block) {
  int ph_block;
  int lb;
  
  ph_block = fs.root[file].first_block;
  for (lb = 0; lb < logic_block; lb++) {
    if (ph_block == EOC) return -1;
    ph_block = fs.block_lists[ph_block];
  }
  
  /* Can return EOC, if caller is write a block must be allocated */
  return ph_block;
}

/* Adds a block to a file and returns a pointer to the ph_block */
int add_block_to_file(int file) {
  int new_block;
  
  if (fs.free_block_count == 0) return -1;
  
  /* Getting a free block */
  new_block = fs.first_free_block;
  /* Updatig list head of free blocks */
  fs.first_free_block = fs.block_lists[new_block];
  /* Assing free block to the file */
  if (fs.root[file].first_block == EOC) {
    /* Is the first block assigned to this file */
    fs.root[file].first_block = new_block;
  } else {
    fs.block_lists[fs.root[file].last_block] = new_block;
  }
  /* Mark the end of file*/
  fs.root[file].last_block = new_block;
  fs.block_lists[fs.root[file].last_block] = EOC;
  fs.used_block_count++;
  fs.free_block_count--;
  
  return new_block;
}

int fat_read_block(struct data_block *block, int ph_block) {
  copy_data((char *)&(fs.data[ph_block]), (char *)block, BLOCK_SIZE);
  return 0;
}

int fat_write_block(struct data_block *block, int ph_block) {
  copy_data((char *) block, (char *)&(fs.data[ph_block]), BLOCK_SIZE);
  return 0;
}

/***********************************************************************
 ************** high level external public FAT functions ***************
 ***********************************************************************/

int check_path(const char *path) {
  int i;
  
  for (i = 0; i < FILE_NAME_SIZE; i++) {
    if (path[i] == '\0') return 0;
  } 
  return -ENAMETOOLONG;  
}


/* Returns file identifier for the given path */
int find_path(const char *path) {
  int i;
  
  for (i = 0; i < MAX_FILES; i++) {
    if (!strcmp(fs.root[i].name, path)) return i;
  }
  
  return -1;
}

/* Deletes a file from FAT metadata */
int delete_file(int file) {  
  
  if (fs.root[file].opens != 0) return -1;
  
  /* Frees all assigned blocks putting them at the end
   * of free list */
  fs.block_lists[fs.last_free_block] = fs.root[file].first_block;
  fs.free_block_count += (fs.root[file].size % BLOCK_SIZE) + 1; 
  
  /* Frees dir entry */
  fs.root[file].mode = FREE_TYPE;
  fs.root[file].size = 0;
  fs.root[file].first_block = EOC;
  fs.root[file].last_block = EOC;
}

int fat_open(int file) {  
  if (file > MAX_FILES) return -1;
  fs.root[file].opens++;
  return 0;
}

int fat_close(int file) {
  if (file > MAX_FILES) return -1;
  if (fs.root[file].opens == 0) return -1;

  fs.root[file].opens--;
  return 0;
}

/* Creates a file in FAT metadata pre-allocating size bytes */
int fat_create(const char *path, int permissions, struct file_operations *fops) {
  int i;
  for (i = 0; i < MAX_FILES; i++) {
    if (fs.root[i].mode == FREE_TYPE) {
      strcat(fs.root[i].name, (char *)path, "");
      fs.root[i].mode = (permissions & O_RDWR);
      fs.root[i].size = 0;
      fs.root[i].fops = fops;
      fs.root[i].first_block = EOC; /* Pointer to first data block */
      fs.root[i].last_block = EOC;
      return i;
    }
  }
  return -1;
}

int fat_read(int file, void *buffer, int offset, int size) {
  int read, remain, block_remain;
  int error;
    
  int ph_block;
  int logic_block;
  /* Offset to read the first block */
  int block_offset;
  /* Ordinal number of firts block needed to read */
  int first_block;
  /* Number of blocks that will be affected by the read */
  int block_count;
    
  //if (offset > f.size) return 0;
    
  first_block = offset / BLOCK_SIZE;
  block_offset = offset - (first_block * BLOCK_SIZE);
  block_count = ((offset + size) / BLOCK_SIZE) - first_block + 1;
    
  read = 0;
  remain = size;
  logic_block = first_block;
  ph_block = fat_translate_block(file, logic_block);
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
    logic_block++;
    
    if (block_count > 0) {
      ph_block = fat_translate_block(file, logic_block);
    }
  }
  return read;
}

int fat_write(int file, void *buffer, int offset, int size) {
  int written, remain, block_remain;
  int error;
      
  int block_num;
  /* Offset to write the first block */
  int block_offset;
  /* Ordinal number of firts block needed to write */
  int first_block;
  /* Number of blocks that will be affected by the write */
  int block_count;
  int ph_block;
      
  
  /* We don't support sparse files */
  //if (offset > f->size) return 0; TODO
    
  first_block = offset / BLOCK_SIZE;
  block_offset = offset - (first_block * BLOCK_SIZE);
  
  block_count = ((offset + size) / BLOCK_SIZE) - first_block;
  if (((offset + size) % BLOCK_SIZE) != 0) block_count++;
    
  written = 0;
  remain = size;
  block_num = first_block;
  ph_block = fat_translate_block(file, block_num);
  
  char msg[80];
  printk("ph_block ");
  itoa(ph_block, msg, 10);
  printk(msg);
  
  
  while (block_count) {
    if (ph_block == EOC) { /* we need to grow! */      
      ph_block = add_block_to_file(file);      
      if (ph_block < 0) return -ENOSPC;
    }
        
    if (remain >= BLOCK_SIZE) {
      block_remain = BLOCK_SIZE - block_offset;
    } else {
      block_remain = remain;
    }
                
    copy_data(buffer + written, block_buffer + block_offset, block_remain);
        
    error = fat_write_block(&block_buffer, ph_block);
    if (error < 0) return error;
    if (block_offset != 0) block_offset = 0; /* Only needed the first time */
      
    fs.root[file].size += block_remain;
    written += block_remain;
    remain -= block_remain;
    block_count--;
    block_num++;
    
    if (block_count > 0) {
      ph_block = fat_translate_block(file, block_num);
    }
  }
  
  return written;
}