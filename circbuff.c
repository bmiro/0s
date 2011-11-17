#include <circbuff.h>

void init_circ_buff(struct circ_buff *cb) {
  cb->head = 0;
  cb->tail = 0;
  cb->count = 0;
}

/* Returns in buffer the amout of charactars specified in size,
   also returns the number of characters read. -1 if error */
int get_character(struct circ_buff *cb, char *dest_buf, int size) {
  int i;
  
  for (i = 0; i < size && !is_empty(cb); i++) {
    dest_buf[i] = cb->mem[cb->head];
    cb->head = (cb->head + 1) % CIRC_BUFF_SIZE;
    cb->count--;
  }
  
  return i;
  
}

/* Puts the character c into the buffer */
int save_character(struct circ_buff *cb, char c) {
  if (is_full(cb)) return -1;
    
  cb->mem[cb->tail] = c;
  cb->tail = (cb->tail + 1) % CIRC_BUFF_SIZE;
  cb->count++;
  
  return 0;
}

char is_empty(struct circ_buff *cb) {
  return cb->head == cb->tail;
}

char is_full(struct circ_buff *cb) {
  return cb->count == CIRC_BUFF_SIZE;
}

int get_size(struct circ_buff *cb) {
  return cb->count;
}