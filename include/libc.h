/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>
#include <errno.h>
#include <entry.h>

#define CONSOLE 1

int perror();
int write(int fd, char *buffer, int size);
int fork(void);
void exit(void);
int getpid(void);

#endif  /* __LIBC_H__ */
