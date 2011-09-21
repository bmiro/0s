/*
 * sys.c - Syscalls implementation
 */

#define SYSBUFF_SIZE 1024

#include <devices.h>
#include <errno.h>

int check_fd(int fd, int mode) {
  if (fd != 1 || mode != WRITE_MODE) {
    return -1;
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
  
  if (check_fd(fd, WRITE_MODE) == -1) return -EBADF;
  if (check_buffer((void*) buffer) == -1) return -EFAULT;
  if (size < 0) return -EINVAL;
    
  copied = 0;
  while (copied < size) {
    chuck = (size - copied) % SYSBUFF_SIZE;
    
    copy_from_user(buffer + copied, sysbuff, chuck);
    switch (fd) {
      case(1):
	      written = sys_write_console(sysbuff, chuck);
	      /* By construction sys_write can NOT fail */
	      copied += written;
      default:
	      break;
    }
  }
  
  return copied;
}

int sys_ni_syscall() {
  return -ENOSYS;
}

