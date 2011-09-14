/*
 * sched.c - initializes struct for task 0
 */

#include <sched.h>
#include <mm.h>

struct protected_task_struct task[NR_TASKS]
  __attribute__((__section__(".data.task")));

void init_task0(void)
{
/* Initializes paging for the process 0 adress space */
  set_user_pages(&task[0].t.task);
  set_cr3();
}
