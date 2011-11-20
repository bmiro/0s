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
    fs.root[i].block_count = 0;
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
  
  return 0;
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
int fat_add_block(int file) {
//   int new_block;
//   
//   if (fs.free_block_count == 0) return -1;
//   
//   /* Getting a free block */
//   new_block = fs.first_free_block;
//   /* Updatig list head of free blocks */
//   fs.first_free_block = fs.block_lists[new_block];
//   /* Assing free block to the file */
//   if (fs.root[file].first_block == EOC) {
//     /* Is the first block assigned to this file */
//     fs.root[file].first_block = new_block;
//   } else {
//     fs.block_lists[fs.root[file].last_block] = new_block;
//   }
//   /* Mark the end of file*/
//   fs.root[file].last_block = new_block;
//   fs.block_lists[fs.root[file].last_block] = EOC;
//   fs.used_block_count++;
//   fs.free_block_count--;
//   
//   return new_block;
  
  if (fs.first_free_block == EOC) return -ENOSPC;
  
  /* Assign block to file */
  if (fs.root[file].first_block == EOC) {
    fs.root[file].first_block = fs.first_free_block;
  } else {
    fs.block_lists[fs.root[file].last_block] = fs.first_free_block;
  }
  fs.root[file].last_block = fs.first_free_block;

  /* Deletes block from free list */
  fs.first_free_block = fs.block_lists[fs.first_free_block];
  if (fs.first_free_block == EOC) {
    fs.last_free_block = EOC;
  }
  
  /* Marks end of the file */
  fs.block_lists[fs.root[file].last_block] = EOC;
  
  fs.used_block_count++;
  fs.free_block_count--;
  
  return fs.root[file].last_block;
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
int fat_check_path(const char *path) {
  int i;
  for (i = 0; i < FILE_NAME_SIZE; i++) {
    if (path[i] == '\0') return 0;
  } 
  return -ENAMETOOLONG;
}

/* Returns file identifier for the given path */
int fat_find_path(const char *path) {
  int i;
 
  for (i = 0; i < MAX_FILES; i++) {
    if (!strcmp(fs.root[i].name, path) && fs.root[i].mode != FREE_TYPE) return i;
  }
  
  return -ENOENT;
}

int fat_get_size(int file) {
  if (0 > file || file > MAX_FILES) return -1;
  return fs.root[file].size;
}

int fat_set_size(int file, int size) {
  if (0 > file || file > MAX_FILES) return -1;
  fs.root[file].size = size;
  return size;
}

int fat_get_fops(int file, struct file_operations **fops) {
  if (0 > file || file > MAX_FILES) return -1;
  
  *fops = fs.root[file].fops;
  return 0;
} 

int fat_get_opens(int file) {
  if (0 > file || file > MAX_FILES) return -1;
  
  return fs.root[file].opens;
}

int fat_open(int file) {  
  if (0 > file || file > MAX_FILES) return -1;
  
  fs.root[file].opens++;
    
  return 0;
}

int fat_close(int file) {
  if (0 > file || file > MAX_FILES) return -1;
  if (fs.root[file].opens == 0) return -1;

  fs.root[file].opens--;
  return 0;
}

/* Creates a file in FAT metadata */
int fat_create(const char *path, int flags, struct file_operations *fops) {
  int i;
  for (i = 0; i < MAX_FILES; i++) {
    if (fs.root[i].mode == FREE_TYPE) {
      strcat(fs.root[i].name, (char *)path, "");
      fs.root[i].mode = (flags & O_RDWR);
      fs.root[i].size = 0;
      fs.root[i].fops = fops;
      fs.root[i].first_block = EOC;
      fs.root[i].last_block = EOC;
      fs.root[i].block_count = 0;
      return i;
    }
  }
  return -ENOSPC;
}

int fat_read(int file, char *buffer, int offset, int count) {
  int remain, read;
  int logic_block, ph_block;
  int block_offset, block_remain;  
 
  if (offset >= fat_get_size(file)) return 0;
  
  if ((offset + count) > fat_get_size(file)) {
    remain = fat_get_size(file) - offset;
  } else {
    remain = count;
  }
  
  read = 0;
  logic_block = offset / BLOCK_SIZE;  
  block_offset = offset % BLOCK_SIZE;
  while (remain) {    
    ph_block = fat_translate_block(file, logic_block);
    if (ph_block == EOC) return read; /* There are problems in FS */
      
    if ((block_offset + remain) > BLOCK_SIZE) {
      block_remain = BLOCK_SIZE - block_offset;
    } else {
      block_remain = remain;
    }
    
    fat_read_block((struct data_block *)block_buffer, ph_block);
    copy_data(block_buffer + block_offset, buffer + read, block_remain);
    
    /* Only needed the first time */
    if (block_offset != 0) block_offset = 0;
    
    read += block_remain;
    remain -= block_remain;
    
    logic_block++;    
  }
  return read;
}


int fat_write(int file, const char *buffer, int offset, int count) {
  int written, remain;
  int logic_block, ph_block;
  int block_offset, block_remain;
  
  written = 0;
  remain = count;
  logic_block = offset / BLOCK_SIZE;
  block_offset = offset % BLOCK_SIZE;
  while (remain) {
    ph_block = fat_translate_block(file, logic_block);
    if (ph_block == EOC) {
      /* File must grow in blocks!!*/
      ph_block = fat_add_block(file);
      if (ph_block < 0) return -ENOSPC;
    }
    
    if ((block_offset + remain) > BLOCK_SIZE) {
      block_remain = BLOCK_SIZE - block_offset;
    } else {
      block_remain = remain;
    }

    fat_read_block((struct data_block *)block_buffer, ph_block);
    copy_data((void *)(buffer + written), block_buffer + block_offset, block_remain);
    fat_write_block((struct data_block *)block_buffer, ph_block);
    
    if (block_offset != 0) block_offset = 0;
    
    written += block_remain;
    remain -= block_remain;
    
    if ((offset + written) > fat_get_size(file)) {
      fat_set_size(file, offset + written);
    }
    
    logic_block++;
  }

  return written;
}

int fat_unlink(int file) {
  if (file > MAX_FILES) return -1;
  if (fs.root[file].opens != 0) return -1;
    
  /* Frees all assigned blocks putting them at the end
   * of free list */
  if (fs.last_free_block != EOC) {
    fs.block_lists[fs.last_free_block] = fs.root[file].first_block;
  } else {
    fs.first_free_block = fs.root[file].first_block;
    fs.last_free_block = fs.root[file].last_block;
  }
  fs.free_block_count += (fs.root[file].size / BLOCK_SIZE);
  fs.used_block_count -= (fs.root[file].size / BLOCK_SIZE);
  if ((fs.root[file].size % BLOCK_SIZE) != 0) {
    fs.free_block_count++;
    fs.used_block_count--;
  }

  /* Frees dir entry */
  strcat(fs.root[file].name, "", "");
  fs.root[file].mode = FREE_TYPE;
  fs.root[file].size = 0;
  fs.root[file].first_block = EOC;
  fs.root[file].last_block = EOC;
    
  int i, b;
  b = fs.first_free_block;
  for(i=0;b != EOC;i++) {
    b = fs.block_lists[b];
  }
  
  printk("free blocks count ");
  char msg[20];
  itoa(i,msg,10);
  printk(msg);
  
  
  return 0;
}