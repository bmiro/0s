/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024

#define PROC_RUNNING 0
#define PROC_READY 1
#define PROC_BLOCKED 2
#define PROC_WAITING 4

struct task_struct {
  unsigned int pid;
  char state;
  char priority;
  unsigned int consumed_ticks;
//   unsigned int owner;
//   unsigned int start_utime;
  struct task_struct *runqueue_next;
  struct task_struct *runqueue_prev;
  struct task_struct *wait_queue_next;
  struct task_struct *wait_queue_prev;
  struct task_struct *blocked_queue_next;
  struct task_struct *blocked_queue_prev;
  struct task_struct *parent;
  struct task_struct *children;
};

union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};

struct protected_task_struct {
  unsigned long task_protect[KERNEL_STACK_SIZE];  /* This field protects the different task_structs, so any acces to this field will generate a PAGE FAULT exeption */
  union task_union t;
};

/** Our sistem can only have NR_TASKS processes,
 * the first one is undeletable and its pointers points
 * to the other ready to run process */
extern struct protected_task_struct task[NR_TASKS];

#define KERNEL_ESP       (DWord) &task[0].t.stack[KERNEL_STACK_SIZE]

/* Inicialitza les dades del proces inicial */
void init_task0(void);

#endif  /* __SCHED_H__ */
