#include <channel.h>

int init_dynamic_channels() {
  int i;
  
  for (i = 0; i < NUM_DYNAMIC_CHANNELS; i++) {
    dyn_channels[i].mode = FREE_CHANNEL;
    dyn_channels[i].opens = 0;
  }
}

int find_free_channel(struct channel *chs) {
  int i;
  
  for (i = 0; i < NUM_CHANNELS; i++) {
    if (chs[i].dynamic == FREE_CHANNEL) return i;
  }
  
  return -1;
}

int find_free_dyn_channel() {
  int i;
  
  for (i = 0; i < NUM_CHANNELS; i++) {
    if (dyn_channels[i].mode == FREE_CHANNEL) return i;
  }
  
  return -1;
}

int file_is_in_use(int file) {
  int i;
  
  for (i = 0; i < NUM_DYNAMIC_CHANNELS; i++) {
    if (dyn_channels[i].file == file) {
      if (dyn_channels[i].opens > 0) return i;
    }
  }
  return -1;
}