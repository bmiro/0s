#ifndef DEVICES_H__
#define  DEVICES_H__

#include <io.h>
#include <channel.h>
#include <sched.h>
#include <circbuff.h>
#include <mm.h>
#include <utils.h>

#define O_RDONLY   0x1
#define O_WRONLY   0x2
#define O_RDWR     (O_RDONLY | O_WRONLY)

struct list_head keyboardqueue;
struct circ_buff circular_buffer;

/* The buffers must be in user-space, the device driver
   must take care to copy data to/from user space */
struct file_operations {
  int (*f_read)(int, void*, int, int);
  int (*f_write)(int, void*, int, int);  
  int (*f_open)(int);
  int (*f_close)(int);
  int (*f_dup)(int);
};

void init_devices();

/* Console */
int sys_write_console(int null0, char *buffer, int null1, int size);

/* Keyboard */
int sys_read_keyboard(int null0, char *buffer, int null1, int size);

/* File */
int sys_open_file(int file);
int sys_write_file(int file, char *buffer, int offset, int size);
int sys_read_file(int file, char *buffer, int offset, int size);


struct file_operations file, console, keyboard;

#endif /* DEVICES_H__*/
