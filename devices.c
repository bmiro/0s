#include <devices.h>

void init_devices() {
  
  initZeOSFAT();

  INIT_LIST_HEAD(&keyboardqueue);
  init_circ_buff(&circular_buffer); 
  
  dev_console.f_read = NULL;
  dev_console.f_write = &sys_write_console;
  dev_console.f_open = NULL;
  dev_console.f_close = NULL;
  dev_console.f_dup = NULL;
  
  dev_keyboard.f_read = &sys_read_keyboard;
  dev_keyboard.f_write = NULL;
  dev_keyboard.f_open = NULL;
  dev_keyboard.f_close = NULL;
  dev_keyboard.f_dup = NULL;
  
  dev_file.f_read = &sys_read_file;
  dev_file.f_write = &sys_write_file;
  dev_file.f_open = &sys_open_file;
  dev_file.f_close = NULL;
  dev_file.f_dup = NULL;
  
  fat_create(KEYBOARD_PATH, O_RDONLY, &dev_keyboard);
  fat_create(CONSOLE_PATH, O_WRONLY, &dev_console);
  
}

void set_default_std_in_out_err(struct task_struct *tsk) {

  tsk->channels[STDIN].fops = &dev_keyboard;
  tsk->channels[STDIN].mode = O_RDONLY;
  tsk->channels[STDIN].offset = 0;
  
  tsk->channels[STDOUT].fops = &dev_console;
  tsk->channels[STDOUT].mode = O_WRONLY;
  tsk->channels[STDOUT].offset = 0;
  
  tsk->channels[STDERR].fops = &dev_console;
  tsk->channels[STDERR].mode = O_WRONLY;
  tsk->channels[STDERR].offset = 0;
  
}

int sys_write_console(int file, const void *buffer, int offset, int size) {
  int chuck, remain;
  int written;
  int i;

  written = 0;
  remain = size;
  while (remain) {
    if (remain < SYSBUFF_SIZE) {
      chuck = remain;
    } else {
      chuck = SYSBUFF_SIZE;
    }
   
    copy_from_user(buffer + written, sysbuff, chuck);
    for (i = 0; i < size; i++) {
      printc(sysbuff[i]);
      written++;
      remain--;
    }
  }

  return written;
}



int sys_read_keyboard(int file, void *buffer, int offset, int size) {
  int read;
  
  if (current()->pid == 0) return -EPERM;
  
  read = 0;
  if (get_size(&circular_buffer) >= size && list_empty(&keyboardqueue)) {
    read += get_character(&circular_buffer, sysbuff, size);
    copy_to_user(sysbuff, buffer, size);
    return read;
  } else {
    current()->offset = 0;
    current()->remain = size;
    current()->buff = buffer;
    
    sched_block(current(), &keyboardqueue);   
    sched_continue(sched_select_next());
  }
  
  return -1; /* We don't arrive here */
  
}

/* File */
int sys_open_file(int file) {
  return fat_open(file);
}

int sys_write_file(int file, const void *buffer, int offset, int count) {
  int chuck, remain;
  int written, wrote;
  
  written = 0;
  remain = count;
  while (remain) {
    chuck = (remain > SYSBUFF_SIZE) ? SYSBUFF_SIZE : remain;
  
    copy_from_user(buffer + written, sysbuff, chuck);
    wrote = fat_write(file, sysbuff, offset + written, chuck);
      
    written += wrote;
    if (wrote != chuck) return written; /* Something happened */
    remain -= wrote;
  }
  return written;
}

int sys_read_file(int file, void *buffer, int offset, int count) {
  int chuck, remain;
  int read, fs_read;
 
  read = 0;
  remain = count;
  while (remain) {
    chuck = (remain > SYSBUFF_SIZE) ? SYSBUFF_SIZE : remain;
    
    fs_read = fat_read(file, sysbuff, offset + read, chuck);
    copy_to_user(sysbuff, buffer + read, fs_read);

    read += fs_read;
    if (fs_read != chuck) return read;
    remain -= fs_read;
  }
  return read;
}
