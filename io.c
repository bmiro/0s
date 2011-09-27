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

void do_scroll() {
  int xx, yy;
    
  Word ch; //= (Word) (c & 0x00FF) | 0x0200;
  DWord screen;
  
  for (yy = 1; yy < NUM_ROWS; yy++) {
    for (xx = 0; xx < NUM_COLUMNS; xx++) {
      /* Reads old character */
      screen = 0xb8000 + (yy * NUM_COLUMNS + xx) * 2;
      asm("movw (%1), %0" : : "r"(ch), "r"(screen));
      
      ch = (Word) (ch & 0x00FF) | 0x0200;
      /* Writes it to the scrolled position */
      screen = 0xb8000 + ((yy-1) * NUM_COLUMNS + xx) * 2;
      asm("movw %0, (%1)" : : "r"(ch), "r"(screen));
    }
  }
  /* Fill last line with blank charaters */
  for (xx = 0; xx < NUM_COLUMNS; xx++) {
    ch = (Word) (' ' & 0x00FF) | 0x0200;
    screen = 0xb8000 + ((yy-1) * NUM_COLUMNS + xx) * 2;
    asm("movw %0, (%1)" : : "r"(ch), "r"(screen));
  }
  
}

/************************** Character printing **************************/
void printc(char c) {
  Word ch = (Word) (c & 0x00FF) | 0x0200;
  DWord screen = 0xb8000 + (y * NUM_COLUMNS + x) * 2;
  
  /* Hack for console printing */
  __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c));

  if (c == '\n') {
    /* Fills line with blank characters */
    while (++x >= NUM_COLUMNS) {
      screen = 0xb8000 + (y * NUM_COLUMNS + x) * 2;
      asm("movw %0, (%1)" : : "r"((' ' & 0x00FF) | 0x0200), "r"(screen));
    }
    x = 0;
    if (++y >= NUM_ROWS) {
      y--;
      do_scroll();
    }
  } else {
    if (++x >= NUM_COLUMNS) {
      x = 0;
      if (++y >= NUM_ROWS) {
	y--;
	do_scroll();
      }
    }
    asm("movw %0, (%1)" : : "r"(ch), "r"(screen));
  }
}

void printc_xy(int xx, int yy, char c) {
  Word ch = (Word) (c & 0x00FF) | 0x0200;
  DWord screen = 0xb8000 + (yy * NUM_COLUMNS + xx) * 2;
  /* No console printing */
  //__asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c));
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

/************************** Other **************************/
void clean_screen() {
  int rw, cl;
  
  for(rw = 0; rw < NUM_ROWS; rw++) {
    for(cl = 0; cl < NUM_COLUMNS; cl++) {
      printc_xy(cl, rw, ' ');
    }
  }
  
}

