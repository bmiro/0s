/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>
#include <errno.h>
#include <entry.h>
#include <file.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

int perror();

int exit();
int fork();

int read(int fd, char *buffer, int size);
int write(int fd, char *buffer, int size);
int open(const char *path, int flags);
int close(int fd);
int unlink(const char *path);

int getpid();

int nice(int quantum);

int sem_init(int n_sem, unsigned int value);
int sem_wait(int n_sem);
int sem_signal(int n_sem);
int sem_destroy(int n_sem);

int get_stats(int pid, struct stats *st);

#endif  /* __LIBC_H__ */
