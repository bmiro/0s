#include <circbuff.h>

char circbuff[CIRC_BUFF_SIZE];
int head, tail;

int init_circ_buff() {
  head = 0;
  tail = 1;
}

/* Returns in buffer the amout of charactars specified in size,
   also returns the number of characters read. -1 if error */
int get_character(char *buffer, int size) {
  
}

/* Puts the character c into the buffer */
int set_character(char c) {
  
}

