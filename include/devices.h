#ifndef DEVICES_H__
#define  DEVICES_H__

#include <io.h>

#define WRITE_MODE 1
#define READ_MODE 0

int sys_write_console(char *buffer, int size);
#endif /* DEVICES_H__*/
