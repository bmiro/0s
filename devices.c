#include <devices.h>

void init_devices() {
  display.f_read = NULL;
  display.f_write = &sys_write_console;
  display.f_open = NULL;
  display.f_close = NULL;
  display.f_dup = NULL;
}

void set_default_std_in_out_err(struct task_struct tsk*) {
  tsk->channels[STDOUT].functions = &display;
  tsk->channels[STDOUT].mode = O_WRONLY;
  tsk->channels[STDOUT].offset = 0;
}

int sys_write_console(char *buffer, int size) {
  int i;

  for (i = 0; i < size; i++) {
    printc(buffer[i]);
  }
  
  return i;
}


int sys_read_keyboard(char *buffer, int size) {
  int s;
  
  return s;
}
