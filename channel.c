#include <file.h>

int find_free_channel(struct channel *chs) {
  int i;
  
  for (i = 0; i < NUM_CHANNELS; i++) {
    if (chs[i].mode == FREE_CHANNEL) return i;
  }
  
  return -1;
}
