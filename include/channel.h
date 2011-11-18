#ifndef CHANNEL_H__
#define  CHANNEL_H__

#define NUM_CHANNELS 10

/** Becareful! this value is realted with O_RDONLY O_WRONLY, they must be distingible */
#define FREE_CHANNEL 0

struct channel {
  struct file_operations *fops;
  int mode; /** O_RDONLY, O_WRONLY, NULL if unopened */
  int file; // File identifier
  int offset; // Read offset
};

int find_free_channel(struct channel *channels);
int is_already_open(struct channel *channels, int file);

#endif