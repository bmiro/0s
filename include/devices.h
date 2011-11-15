#ifndef DEVICES_H__
#define  DEVICES_H__

#include <io.h>
#include <file.h>
#include <sched.h>

struct file_operations {
  int (*f_read)(void*, void*, int);
  int (*f_write)(void*, int);  
  int (*f_open)(int);
  int (*f_close)(int);
  int (*f_dup)(int);
};

void init_devices();

int sys_write_console(char *buffer, int size);
int sys_read_keyboard(char *buffer, int size);

struct file_operations file, display, keyboard;

#endif /* DEVICES_H__*/
