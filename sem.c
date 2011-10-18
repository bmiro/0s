#include <sem.h>

struct sem_struct sems[NR_SEM]
  __attribute__((__section__(".data.sems")));

void init_sems() {
  int i;
  for (i=0; i < NR_SEM; i++) {
    sems[i].owner = FREE_SEM;
  }
}

