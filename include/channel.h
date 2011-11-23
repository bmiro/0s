#ifndef CHANNEL_H__
#define  CHANNEL_H__

#define NUM_CHANNELS 10
#define NUM_DYNAMIC_CHANNELS NUM_CHANNELS*10

/** Becareful! this value is realted with O_RDONLY O_WRONLY, they must be distingible */
#define FREE_CHANNEL -1

struct channel {
  int dynamic; /* FREE_CHANNEL if unopened */
};

struct dyn_channel {
  struct file_operations *fops;
  int file; /* File identifier */
  int mode; /* O_RDONLY, O_WRONLY, FREE_CHANNEL if unopened */
  int offset;
  int opens;
};

struct dyn_channel dyn_channels[NUM_DYNAMIC_CHANNELS];

int init_dynamic_channels();
int find_free_channel(struct channel *chs);
int find_free_dyn_channel();
int file_is_in_use(int file);

#endif