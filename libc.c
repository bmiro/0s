
/*
 * libc.c 
 */

#include <libc.h>

/* Caution!! this mask two parameters in one */
#define WELL_FORMED_STR(x) (x, sizeof(x))

int errno;

int perror() {
  /* TODO llista d'errors */
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
  
  __asm__ __volatile__(
    "popl %edx\n"
    "popl %ecx\n"
    "popl %ebx\n"
    "movl $4, %eax\n" 
    "int $0x80\n"
    "movl %%eax, %0\n"
    :
    : "g" (error));
    
    if (error > -1) {
      /* Successful syscall */
      return error;
    } else {
      errno = -error;
      return -1;
    }
    
    
}
