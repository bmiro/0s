#ifndef FILE_H__
#define  FILE_H__

#define NUM_CHANNELS 10

/** Becareful! this value is realted with O_RDONLY O_WRONLY, they must be distingible */
#define FREE_CHANNEL 0

#define DEFAULT_STDIN  0
#define DEFAULT_STDOUT 1
#define DEFAULT_STDERR 2

struct channel {
  struct file_operations *functions;
  char mode; /** O_RDONLY, O_WRONLY, NULL if unopened */
  int file; // File identifier
  int offset; // Read offset
};

int find_free_channel(struct channel *channels);

#endif