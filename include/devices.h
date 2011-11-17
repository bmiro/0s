#ifndef DEVICES_H__
#define  DEVICES_H__

#include <io.h>
#include <channel.h>
#include <sched.h>
#include <circbuff.h>
#include <mm.h>
#include <utils.h>

struct list_head keyboardqueue;
struct circ_buff circular_buffer;

struct file_operations {
  int (*f_read)(void*, int, int);
  int (*f_write)(void*, int, int);  
  int (*f_open)(int);
  int (*f_close)(int);
  int (*f_dup)(int);
};

void init_devices();

/* Console */
int sys_write_console(char *buffer, int offset, int size);

/* Keyboard */
int sys_read_keyboard(char *buffer, int offset, int size);

/* File */
int sys_write_file(char *buffer, int offset, int size);
int sys_read_file(char *buffer, int offset, int size);


struct file_operations file, console, keyboard;

#endif /* DEVICES_H__*/
