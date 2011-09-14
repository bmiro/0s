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
Byte inb (unsigned short port)
{
  Byte v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (v):"Nd" (port));
  return v;
}

void printc(char c)
{
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

void printk(char *string)
{
  int i;
  for (i = 0; string[i]; i++)
    printc(string[i]);
}
