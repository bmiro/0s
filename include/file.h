#ifndef FILE_H__
#define  FILE_H__

/** Becareful! this value is realted with O_RDONLY O_WRONLY, they must be distingible */
#define FREE_CHANNEL 0

struct file_operations {
  int (*f_read)(void*, int);
  int (*f_write)(void*, int);  
  int (*f_open)(int);
  int (*f_close)(int);
  int (*f_dup)(int);
}

struct channel {
  struct file_operation *functions;
  char mode; /** O_RDONLY, O_WRONLY, NULL if unopened */
  int file; // File identifier
  int offset; // Read offset
}

int find_free_channel(struct fds*) {
  int i;
  
  for (i = 0; i < NUM_CHANNELS; i++) {
    if (fds[i]->mode == FREE_CHANNEL) return i;
  }
  
  return -1;
}

#endif