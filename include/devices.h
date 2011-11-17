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
  int (*f_read)(void*, int);
  int (*f_write)(void*, int);  
  int (*f_open)(int);
  int (*f_close)(int);
  int (*f_dup)(int);
};

void init_devices();

/* Console */
int sys_write_console(char *buffer, int size);

/* Keyboard */
int sys_read_keyboard(char *buffer, int size);

struct file_operations file, console, keyboard;

#endif /* DEVICES_H__*/
