#include <channel.h>

int find_free_channel(struct channel *chs) {
  int i;
  
  for (i = 0; i < NUM_CHANNELS; i++) {
    if (chs[i].dyn_chars == FREE_CHANNEL) return i;
  }
  
  return -1;
}

int find_free_dyn_channel(struct dyn_channel *dchs) {
  int i;
  
  for (i = 0; i < NUM_CHANNELS; i++) {
    if (dchs[i].mode == FREE_CHANNEL) return i;
  }
  
  return -1;
}
