#ifndef __CIRCBUFF_H__
#define __CIRCBUFF_H__

#define CIRC_BUFF_SIZE 80

int init_circ_buff();

/* Returns in buffer the amout of charactars specified in size,
   also returns the number of characters read. -1 if error */
int get_character(char *buffer, int size);

/* Puts the character c into the buffer */
int set_character(char c);

#endif