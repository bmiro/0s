#include <devices.h>

void init_devices() {
  
}

void set_default_std_in_out_err(struct task_struct tsk*) {
  tsk->channels[STDOUT].functions
  tsk->channels[STDOUT].mode
  tsk->channels[STDOUT].file
  tsk->channels[STDOUT].file
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
