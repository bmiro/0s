
/*
 * libc.c 
 */

#include <libc.h>
#include <errno.h>

/* Caution!! this mask two parameters in one */
#define WELL_FORMED_STR(x) x, sizeof(x)

int errno;

int perror() {
  switch (errno) {
    case (EPERM) :
      write(CONSOLE, WELL_FORMED_STR("[perror] Operation not permitted.\n"));
      break;
    case (ESRCH) :
      write(CONSOLE, WELL_FORMED_STR("[perror] No such process.\n"));
      break; 
    case (EBADF) :
      write(CONSOLE, WELL_FORMED_STR("[perror] Bad file number.\n"));
      break;
    case (EAGAIN) :
      write(CONSOLE, WELL_FORMED_STR("[perror] Try again.\n"));
      break;
    case (ENOMEM) :
      write(CONSOLE, WELL_FORMED_STR("[perror] Out of memory.\n"));
      break;
    case (EACCES) :
      write(CONSOLE, WELL_FORMED_STR("[perror] Permission denied.\n"));
      break;
    case (EFAULT) :
      write(CONSOLE, WELL_FORMED_STR("[perror] Bad address.\n"));
      break;
    case (EBUSY) :
      write(CONSOLE, WELL_FORMED_STR("[perror] Device or resource busy.\n"));
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

int exit(void) {
  int error;
  int id = SYS_EXIT_ID;

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

int nice(int quantum) {
  int error;
  int id = SYS_NICE_ID;

  __asm__ __volatile__(
    "movl %2, %%eax\n" 
    "int $0x80\n"
    "movl %%eax, %0\n"
    : "=a" (error)
    : "b" (quantum), "g" (id)
  );
  
  if (error < 0) {
    errno = -error;
    return -1;
  } else {
    /* Successful syscall */
    return error;
  } 
  
}

int sem_init(int n_sem, unsigned int value) {
  int error;
  int id = SYS_SEM_INIT_ID;

  __asm__ __volatile__(
    "movl %3, %%eax\n" 
    "int $0x80\n"
    "movl %%eax, %0\n"
    : "=a" (error)
    : "b" (n_sem), "c" (value), "g" (id)
  );
  
  if (error < 0) {
    errno = -error;
    return -1;
  } else {
    /* Successful syscall */
    return error;
  } 
  
}

int sem_wait(int n_sem) {
  int error;
  int id = SYS_SEM_WAIT_ID;

  __asm__ __volatile__(
    "movl %2, %%eax\n" 
    "int $0x80\n"
    "movl %%eax, %0\n"
    : "=a" (error)
    : "b" (n_sem), "g" (id)
  );
  
  if (error < 0) {
    errno = -error;
    return -1;
  } else {
    /* Successful syscall */
    if (error == 0) {
      write(1, "0!\n", 6);
    }
    return error;
  } 
  
}

int sem_signal(int n_sem) {
  int error;
  int id = SYS_SEM_SIGNAL_ID;

  __asm__ __volatile__(
    "movl %2, %%eax\n" 
    "int $0x80\n"
    "movl %%eax, %0\n"
    : "=a" (error)
    : "b" (n_sem), "g" (id)
  );
  
  if (error < 0) {
    errno = -error;
    return -1;	
  } else {
    /* Successful syscall */
    return error;
  } 
  
}

int sem_destroy(int n_sem) {
  int error;
  int id = SYS_SEM_DESTROY_ID;

  __asm__ __volatile__(
    "movl %2, %%eax\n" 
    "int $0x80\n"
    "movl %%eax, %0\n"
    : "=a" (error)
    : "b" (n_sem), "g" (id)
  );
  
  if (error < 0) {
    errno = -error;
    return -1;
  } else {
    /* Successful syscall */
    return error;
  } 
  
}

int get_stats(int pid, struct stats *st) {
  int error;
  int id = SYS_GET_STATS_ID;

  __asm__ __volatile__(
    "movl %3, %%eax\n" 
    "int $0x80\n"
    "movl %%eax, %0\n"
    : "=a" (error)
    : "b" (pid), "c" (st), "g" (id)
  );
  
  if (error < 0) {
    errno = -error;
    return -1;
  } else {
    /* Successful syscall */
    return error;
  }
}

/* Internal syscall for debug only */
int debug() {
  int error;
  int id = SYS_DEBUG_ID;

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