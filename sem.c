#include <sem.h>

void init_sems() {
  int i;
  for (i=0; i < NR_SEM; i++) {
    sems[i].owner = FREE_SEM;
  }
}

