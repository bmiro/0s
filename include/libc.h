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
int fork();
int exit();
int getpid();

int nice(int quantum);
int sem_init(int n_sem, unsigned int value);
int sem_wait(int n_sem);
int sem_signal(int n_sem);
int sem_destroy(int n_sem);
int get_stats(int pid, struct stats *st);

#endif  /* __LIBC_H__ */
