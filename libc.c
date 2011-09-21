
/*
 * libc.c 
 */

#include <libc.h>

/* Caution!! this mask two parameters in one */
#define WELL_FORMED_STR(x) x, sizeof(x)

int errno;

int perror() {
  switch (errno) {
    case (0) :
      break;
    default :
        write(CONSOLE, WELL_FORMED_STR("Unknown error, fuck off!"));
	break;
  }
  
  return errno;
}

/* Wrapper of write system call*/
int write(int fd, char *buffer, int size) {
  int error;
  int id = SYS_WRITE_ID;

    __asm__ __volatile__(
      "movl %4, %%eax\n" 
      "int $0x80\n"
      "movl %%eax, %0\n"
      : "=a" (error)
      : "b" (fd), "c" (buffer), "d" (size), "g" (id)
    );
    
    if (error < 0) {
      errno = -error;
      return -1;
    } else {
      /* Successful syscall */
      return error;
    } 
}
