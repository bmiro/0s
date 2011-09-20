/*
 * io.h - Definició de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#define KEYBOARD_PORT 0x60
#define KEY_MAKE 0
#define KEY_BREAK 1

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

#endif  /* __IO_H__ */
