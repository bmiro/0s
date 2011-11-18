#include <channel.h>

int find_free_channel(struct channel *channels) {
  int i;
  
  for (i = 0; i < NUM_CHANNELS; i++) {
    if (channels[i].mode == FREE_CHANNEL) return i;
  }
  
  return -1;
}

int is_already_open(struct channel *channels, int file) {
  int i;
  
  for (i = 0; i < NUM_CHANNELS; i++) {
    if (channels[i].file == file) return i;
  }
  
  return 0;
}
