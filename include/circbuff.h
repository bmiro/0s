#ifndef __CIRCBUFF_H__
#define __CIRCBUFF_H__

#define CIRC_BUFF_SIZE 80

struct circ_buff;

int init_circ_buff(struct circ_buff *cb);

/* Returns in buffer the amout of charactars specified in size,
   also returns the number of characters read. -1 if error */
int get_character(struct circ_buff *cb, char *dest_buff, int size);

/* Puts the character c into the buffer */
int set_character(struct circ_buff *cb, char c);

char is_empty(struct circ_buff *cb);

char is_full(struct circ_buff *cb);

#endif