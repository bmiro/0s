#include <devices.h>

int sys_write_console(char *buffer, int size) {
  int i;

  for (i=0; i < size; i++) {
    printc(buffer[i]);
  }
  
  return i;
}
