/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <mm_address.h>

#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024

#define TASK_RUNNING 0
#define TASK_READY 1
#define TASK_BLOCKED 2
#define TASK_WAITING 3
#define TASK_FREE 4

#define FULL_QUANTUM 8

struct task_struct {
  unsigned int pid; /* Keep it the first member */
  char state;
  char quantum;
  struct list_head queue;
  
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

void init_queues(void);

static inline struct task_struct* list_head_to_task_struct(struct list_head *l) {
  return list_entry(l, struct task_struct, queue);
}

struct task_struct* current();

#endif  /* __SCHED_H__ */
