/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <stats.h>
#include <mm_address.h>

#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024

#define NULL_PID -1
#define NULL_TSK -1

#define TASK_RUNNING 0
#define TASK_READY 1
#define TASK_BLOCKED 2
#define TASK_WAITING 3
#define TASK_FREE 4

#define DEFAULT_QUANTUM 50

struct list_head runqueue;
  
struct task_struct {
  unsigned int pid; /* Keep it the first member */
  char quantum;
  struct list_head queue;
  struct stats st;
  
  //unsigned int consumed_tics;
  
  //struct task_struct *parent;
  //struct task_struct *children;
  //struct task_struct *brotherhood; //TODO emprar llenguatge no sexista
  unsigned int phpages[NUM_PAG_CODE + NUM_PAG_DATA];
  
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

void init_task_structs(void);

void init_queues(void);

static inline struct task_struct* list_head_to_task_struct(struct list_head *l) {
  return list_entry(l, struct task_struct, queue);
}

struct task_struct* current();

/* Returns the task struct corresponding to that pid, NULL_TSK if pid does NOT exist */
struct task_struct* getTS(int pid);
void task_switch(union task_union *t);

void sched_update_status();
char sched_switch_needed();
struct task_struct* sched_select_next();
void sched_pause(struct task_struct *tsk);
void sched_continue(struct task_struct *tsk);
void sched_block(struct task_struct *tsk, struct list_head *queue);
void sched_unblock(struct task_struct *tsk);

#endif  /* __SCHED_H__ */
