#ifndef DEVICES_H__
#define  DEVICES_H__

#include <io.h>
#include <channel.h>
#include <sched.h>
#include <circbuff.h>
#include <mm.h>
#include <utils.h>
#include <libc.h>
#include <fat.h>

#define KEYBOARD_PATH "KEYBOARD"
#define CONSOLE_PATH "DISPLAY"

struct list_head keyboardqueue;
struct circ_buff circular_buffer;

/* The buffers must be in user-space, the device driver
   must take care to copy data to/from user space */
struct file_operations {
  int (*f_read)(int, void*, int, int);
  int (*f_write)(int, void*, int, int);  
  int (*f_open)(int);
  int (*f_close)(int);
  int (*f_unlink)(int);
  int (*f_dup)(int);
};

void init_devices();

/* Console */
int sys_write_console(int file, const void *buffer, int offset, int count);

/* Keyboard */
int sys_read_keyboard(int file, void *buffer, int offset, int count);

/* File */
int sys_open_file(int file);
int sys_close_file(int file);
int sys_write_file(int file, const void *buffer, int offset, int count);
int sys_read_file(int file, void *buffer, int offset, int count);
int sys_unlink_file(int file);

struct file_operations dev_file, dev_console, dev_keyboard;

#endif /* DEVICES_H__*/
