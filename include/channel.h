#ifndef CHANNEL_H__
#define  CHANNEL_H__

#define NUM_CHANNELS 10

/** Becareful! this value is realted with O_RDONLY O_WRONLY, they must be distingible */
#define FREE_CHANNEL -1

struct channel {
  struct file_operations *fops;
  int file; // File identifier
  int dyn_chars; /* FREE_CHANNEL if unopened */
};

struct dyn_channel {
  int mode; /* O_RDONLY, O_WRONLY, FREE_CHANNEL if unopened */
  int offset;
};

int find_free_channel(struct channel *chs);
int find_free_dyn_channel(struct dyn_channel *dchs);

#endif