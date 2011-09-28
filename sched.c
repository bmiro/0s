/*
 * sched.c - initializes struct for task 0
 */

#include <sched.h>
#include <list.h>
#include <mm.h>

struct protected_task_struct task[NR_TASKS]
  __attribute__((__section__(".data.task")));

struct list_head runqueue;
struct list_head blockedqueue;
struct list_head waitqueue;
struct list_head termintatedqueue;

  
void init_queues () {
  //TODO
}
   
void init_task0(void) {
/* Initializes paging for the process 0 adress space */
  set_user_pages(&task[0].t.task);
  set_cr3();
  
  
}
