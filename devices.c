#include <devices.h>

void init_devices() {
  console.f_read = NULL;
  console.f_write = &sys_write_console;
  console.f_open = NULL;
  console.f_close = NULL;
  console.f_dup = NULL;
}

void set_default_std_in_out_err(struct task_struct *tsk) {
  tsk->channels[STDOUT].functions = &console;
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
