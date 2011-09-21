/*
 * sys.c - Syscalls implementation
 */

#define SYSBUFF_SIZE 1024

#include <devices.h>

int check_fd(int fd, int mode) {
  if (fd != 1 && mode == WRITE_MODE) {
    return -1; /* TODO verificar error */
  } else {
    return 0;
  }
}

int check_buffer(void *buffer) {
    if (buffer == NULL) {
      return -1;
    } else {
      return 0;
    }
}

int sys_write(int fd, char *buffer, int size) {
  int error;
  char sysbuff[SYSBUFF_SIZE];
  int chuck, copied;
  int written;
  
  if (check_fd(fd) == -1) {
    error = -1; /* Comprovar codi error */
    return error;
  }
  
  if (check_buffer((void*) buffer)) {
    error = -1;
    return error;
  }
  
  if (size < 0) {
    error = -1;
    return error;
  }
  
  copied = 0;
  while (copied < size) {
    chuck = (size - copied) % SYSBUFF_SIZE;
    
    copy_from_user(buffer + copied, sysbuff, chuck);
    switch (fd) {
      case(1):
	      written = sys_write_console(sysbuff, chuck);
	      /* TODO comprovació d'errors */
	      copied += written;
      default:
	      break;
    }
  }
  
  return size;
}

int sys_ni_syscall() {
  return -1;
}

