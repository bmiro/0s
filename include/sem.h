#ifndef __SEM_H__
#define __SEM_H__

#include <list.h>

#define NR_SEM 10

#define FREE_SEM -1

struct sem_struct {
  int owner;
  int value;
  struct list_head queue;
};

extern struct sem_struct sems[NR_SEM];

void init_sems();

#endif