#include <circbuff.h>

struct circ_buff {
  char mem[CIRC_BUFF_SIZE];
  int head, tail;
};

int init_circ_buff(struct circ_buff *cb) {
  cb.head = 0;
  cb.tail = 0;
}

/* Returns in buffer the amout of charactars specified in size,
   also returns the number of characters read. -1 if error */
int get_character(struct circ_buff *cb, char *dest_buf, int size) {
  
}

/* Puts the character c into the buffer */
int set_character(char c) {
  
}

char is_empty(struct circ_buff *cb) {
  return head == tail;
}

char is_full(struct circ_buff *cb) {
  return ((tail + 1) % (CIRC_BUFF_SIZE-1)) == head;
}

