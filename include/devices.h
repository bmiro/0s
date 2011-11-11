#ifndef DEVICES_H__
#define  DEVICES_H__

#include <io.h>

struct file_operations {
  int (*f_read)(int, char*, int);
  int (*f_write)(int, char*, int);  
  int (*f_open)(int);
  int (*f_close)(int);
  int (*f_dup)(int);
}

struct virtual_device {
  const char name[FILE_NAME_SIZE];
  const char valid_modes;
  const struct file_operations fops*;
  int seek;
  char open_mode;
}

int sys_write_console(char *buffer, int size);
int sys_read_keyboard(char *buffer, int size);

#endif /* DEVICES_H__*/
