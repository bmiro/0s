/*
 * io.h - Definició de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#define O_RDONLY   0x1
#define O_WRONLY   0x2
#define O_RDWR     (O_RDONLY | O_WRONLY)

#define DEFAULT_STDIN  0
#define DEFAULT_STDOUT 1
#define DEFAULT_STDERR 2

#define KEYBOARD_PORT 0x60
#define KEY_MAKE      0
#define KEY_BREAK     1

#define VIRTUAL_KEYBOARD_PATH "KEYBOARD"
#define VIRTUAL_DISPLAY_PATH "DISPLAY"

#include <types.h>

/** Screen functions **/
/**********************/

Byte inb (unsigned short port);

/* Character printing */
void printc(char c);
void printc_xy(int xx, int yy, char c);

/* String printing */
void printk(char *string);
void printk_xy(int xx, int yy, char *str);
void printk_xyr(int xx, int yy, char *str);

/* Other */
void clean_screen();

#endif  /* __IO_H__ */
