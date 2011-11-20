
/*
 * libc.c 
 */

#include <libc.h>
#include <errno.h>

/* Caution!! this mask two parameters in one */
#define WELL_FORMED_STR(x) x, sizeof(x)

#define no_arg_syscall(id) \
  int error; \
  __asm__ __volatile__( \
    "movl %1, %%eax\n" \
    "int $0x80\n" \
    "movl %%eax, %0\n" \
    : "=a" (error) \
    : "g" (id) \
  ); \
  if (error < 0) { \
    errno = -error; \
    return -1; \
  } else { \
    /* Successful syscall */ \
    return error; \
  } \

#define one_arg_syscall(id, arg0) \
  int error; \
  __asm__ __volatile__( \
    "movl %2, %%eax\n" \
    "int $0x80\n" \
    "movl %%eax, %0\n" \
    : "=a" (error) \
    : "b" (arg0), "g" (id) \
  ); \
  if (error < 0) { \
    errno = -error; \
    return -1; \
  } else { \
    /* Successful syscall */ \
    return error; \
  } \
  
#define two_arg_syscall(id, arg0, arg1) \
  int error; \
  __asm__ __volatile__( \
    "movl %3, %%eax\n" \
    "int $0x80\n" \
    "movl %%eax, %0\n" \
    : "=a" (error) \
    : "b" (arg0), "c" (arg1), "g" (id) \
  ); \
  if (error < 0) { \
    errno = -error; \
    return -1; \
  } else { \
    /* Successful syscall */ \
    return error; \
  } \
  
#define three_arg_syscall(id, arg0, arg1, arg2) \
  int error; \
  __asm__ __volatile__( \
    "movl %4, %%eax\n" \
    "int $0x80\n" \
    "movl %%eax, %0\n" \
    : "=a" (error) \
    : "b" (arg0), "c" (arg1), "d" (arg2), "g" (id) \
  ); \
  if (error < 0) { \
    errno = -error; \
    return -1; \
  } else { \
    /* Successful syscall */ \
    return error; \
  } \

unsigned int errno;

int perror() {
  switch (errno) {
    case (EPERM) :
      write(STDERR, WELL_FORMED_STR("[perror] Operation not permitted.\n"));
      break;
    case (ENOENT) :
      write(STDERR, WELL_FORMED_STR("[perror] No such file or directory.\n"));
      break;
    case (ESRCH) :
      write(STDERR, WELL_FORMED_STR("[perror] No such process.\n"));
      break; 
    case (EBADF) :
      write(STDERR, WELL_FORMED_STR("[perror] Bad file number.\n"));
      break;
    case (EAGAIN) :
      write(STDERR, WELL_FORMED_STR("[perror] Try again.\n"));
      break;
    case (ENOMEM) :
      write(STDERR, WELL_FORMED_STR("[perror] Out of memory.\n"));
      break;
    case (EACCES) :
      write(STDERR, WELL_FORMED_STR("[perror] Permission denied.\n"));
      break;
    case (EFAULT) :
      write(STDERR, WELL_FORMED_STR("[perror] Bad address.\n"));
      break;
    case (EBUSY) :
      write(STDERR, WELL_FORMED_STR("[perror] Device or resource busy.\n"));
      break;
    case (EEXIST) :
      write(STDERR, WELL_FORMED_STR("[perror] File exists.\n"));
      break;   
    case (EINVAL) :
      write(STDERR, WELL_FORMED_STR("[perror] Invalid argument.\n"));
      break;
    case (EMFILE) :
      write(STDERR, WELL_FORMED_STR("[perror] Too many open files.\n"));
      break;
    case (ENOSPC) :
      write(STDERR, WELL_FORMED_STR("[perror] No space left on device.\n"));
      break;
    case (ENAMETOOLONG) :
      write(STDERR, WELL_FORMED_STR("[perror] File name too long.\n"));
      break;
    case (ENOSYS) :
      write(STDERR, WELL_FORMED_STR("[perror] Function not implemented.\n"));
      break; 
    default :
      write(STDERR, WELL_FORMED_STR("[perror] Unknown error, fuck off!\n"));
      break;
  }
  
  return errno;
}

int exit(void) {
  no_arg_syscall(SYS_EXIT_ID);
}

int fork(void) {
  no_arg_syscall(SYS_FORK_ID);
}

int read(int fd, char *buffer, int size) {
  three_arg_syscall(SYS_READ_ID, fd, buffer, size);
}

int write(int fd, char *buffer, int size) {
  three_arg_syscall(SYS_WRITE_ID, fd, buffer, size);
}

int open(const char *path, int flags) {
  two_arg_syscall(SYS_OPEN_ID, path, flags);
}

int close(int fd) {
  one_arg_syscall(SYS_CLOSE_ID, fd)
}

int unlink(const char *path) {
  one_arg_syscall(SYS_UNLINK_ID, path);
}

int getpid(void) {
  no_arg_syscall(SYS_GETPID_ID);
}

int sem_init(int n_sem, unsigned int value) {
  two_arg_syscall(SYS_SEM_INIT_ID, n_sem, value);
}

int sem_wait(int n_sem) {
  one_arg_syscall(SYS_SEM_WAIT_ID, n_sem);
}

int sem_signal(int n_sem) {
  one_arg_syscall(SYS_SEM_SIGNAL_ID, n_sem);
}

int sem_destroy(int n_sem) {
  one_arg_syscall(SYS_SEM_DESTROY_ID, n_sem);
}

int nice(int quantum) {
  one_arg_syscall(SYS_NICE_ID, quantum)
}

int get_stats(int pid, struct stats *st) {
  two_arg_syscall(SYS_GET_STATS_ID, pid, st);
}

int dup(int fd) {
  one_arg_syscall(SYS_DUP_ID, fd)
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