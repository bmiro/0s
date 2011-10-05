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
struct list_head terminatedqueue;

#define CURRENT_TASK_MASK 0xFFFFF000
#define ACCES_TS(x, y) task[x].t.task.y

struct task_struct* current() {
  unsigned long sp;
  
  __asm__ __volatile__(
      "movl %%esp, %0\n"
      : "=a" (sp)
      :
  );
  
  /* 2 Because we use empty protected blocks to control stack overflows */
  return (struct task_struct*)((sp / KERNEL_STACK_SIZE*2) & CURRENT_TASK_MASK);
}
  
void init_queues(void) {
  INIT_LIST_HEAD(&runqueue);
  INIT_LIST_HEAD(&blockedqueue);
  INIT_LIST_HEAD(&terminatedqueue);
}
   
void init_task0(void) {
  struct task_struct ts;
  ts = task[0].t.task;

  /* Initializes paging for the process 0 adress space */
  set_user_pages(&ts);
  set_cr3();
  
  ts.pid = 0; /* Setting PID 0 */
  ts.quantum = FULL_QUANTUM;
  ts.state = TASK_RUNNING;
  list_add(&ts.queue, &runqueue);
  
  
  
  
  
  
  
  
  
}
