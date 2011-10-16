#ifndef __SCHED_H__
#define __SCHED_H__

#define NR_SEM 10

#define FREE_SEM

struct sem_struct {
  int owner;
  int value;
  struct list_head queue;
};

extern struct sem_struct sems[NR_SEM];

#endif