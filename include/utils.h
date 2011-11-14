#ifndef UTILS_H
#define UTILS_H

#define READ 0
#define WRITE 1

void copy_data(void *start, void *dest, int size);
int copy_from_user(void *start, void *dest, int size);
int copy_to_user(void *start, void *dest, int size);

int access_ok(int type, const void *addr, unsigned long size);

#endif
