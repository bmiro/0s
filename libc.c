
/*
 * libc.c 
 */

#include <libc.h>

/* Wrapper of write system call*/
int write(int fd, char *buffer, int size) {
  
  /*TODO*/
  __asm__ __volatile__(
    "popl %edx\n"
    "popl %ecx\n"
    "popl %ebx\n"
    "movl $4, %eax\n" 
    "int $0x80\n");
    
    /* Errno */
    
    
}

