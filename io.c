/*
 * io.c - 
 */

#include <io.h>

/**************/
/** Screen  ***/
/**************/

#define NUM_COLUMNS 80
#define NUM_ROWS    25

Byte x, y=15;

/* Read a byte from 'port' */
Byte inb (unsigned short port) {
  Byte v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (v):"Nd" (port));
  return v;
}

/************************** Character printing **************************/
void printc(char c) {
  Word ch = (Word) (c & 0x00FF) | 0x0200;
  DWord screen = 0xb8000 + (y * NUM_COLUMNS + x) * 2;
   __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c));
  if (++x >= NUM_COLUMNS)
  {
    x = 0;
    if (++y >= NUM_ROWS)
      y = 0;
  }
  asm("movw %0, (%1)" : : "r"(ch), "r"(screen));
}

void printc_xy(int xx, int yy, char c) {
  Word ch = (Word) (c & 0x00FF) | 0x0200;
  DWord screen = 0xb8000 + (yy * NUM_COLUMNS + xx) * 2;
   __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c));
  asm("movw %0, (%1)" : : "r"(ch), "r"(screen));
}


/************************** String Printing **************************/
void printk_xy(int xx, int yy, char *str) {
  int i;
  
  for (i = 0; str[i]; i++) {
    if (++xx >= NUM_COLUMNS) {
      xx = 0;
    if (++yy >= NUM_ROWS)
      yy = 0;
    }
    printc_xy(xx, yy, str[i]);
  }
}

/* xx unsused, always form the last column */
void printk_xyr(int xx, int yy, char *str) {
  int i, l;
  
  for (l=0; str[l]; l++); /* l contains str lengh */
  for (i=0; i <= l; i++) {
    printc_xy(NUM_COLUMNS-i, yy, str[l-i]);
  }
}

void printk(char *string) {
  int i;
  for (i = 0; string[i]; i++) {
    printc(string[i]);
  }
}
