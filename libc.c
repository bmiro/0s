
/*
 * libc.c 
 */

#include <libc.h>

/* Caution!! this mask two parameters in one */
#define WELL_FORMED_STR(x) x, sizeof(x)

int errno;

#define EBADF        9  /* Bad file number */
#define EFAULT      14  /* Bad address */
#define EINVAL      22  /* Invalid argument */
#define ENOSYS      38  /* Function not implemented */


int perror() {
  switch (errno) {
    case (EBADF) :
      write(CONSOLE, WELL_FORMED_STR("[perror] Bad file number.\n"));
      break;
    case (EAGAIN) :
      write(CONSOLE, WELL_FORMED_STR("[perror] Try again.\n"));
      break;
    case (EFAULT) :
      write(CONSOLE, WELL_FORMED_STR("[perror] Bad address.\n"));
      break;
    case (EINVAL) :
      write(CONSOLE, WELL_FORMED_STR("[perror] Invalid argument.\n"));
      break;
    case (ENOSYS) :
      write(CONSOLE, WELL_FORMED_STR("[perror] Function not implemented.\n"));
      break; 
    default :
      write(CONSOLE, WELL_FORMED_STR("[perror] Unknown error, fuck off!\n"));
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

int fork(void) {
  int error;
  int id = SYS_FORK_ID;

  __asm__ __volatile__(
    "movl %1, %%eax\n" 
    "int $0x80\n"
    "movl %%eax, %0\n"
    : "=a" (error)
    : "g" (id)
  );

  if (error < 0) {
    errno = -error;
    return -1;
  } else {
    /* Successful syscall */
    return error;
  }
  
}

int getpid(void) {
  int error;
  int id = SYS_GETPID_ID;

  __asm__ __volatile__(
    "movl %1, %%eax\n" 
    "int $0x80\n"
    "movl %%eax, %0\n"
    : "=a" (error)
    : "g" (id)
  );

  if (error < 0) {
    errno = -error;
    return -1;
  } else {
    /* Successful syscall */
    return error;
  }
}
  
