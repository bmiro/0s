/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <stats.h>
#include <mm_address.h>
#include <channel.h>

#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024
#define EAX_POS 10

#define NULL_PID -1
#define NULL_TSK -1

#define TASK_RUNNING 0
#define TASK_READY 1
#define TASK_BLOCKED 2
#define TASK_WAITING 3
#define TASK_FREE 4

#define DEFAULT_QUANTUM 10

struct list_head runqueue;
  
struct task_struct {
  /* Process IDentifier */
  unsigned int pid;
  
  /* scheduling */
  char state;
  struct stats st;
  unsigned int quantum; 
  struct list_head queue;
  
  /* Memory */
  unsigned int phpages[NUM_PAG_CODE + NUM_PAG_DATA];
  
  /* I/O */
  char *buff; /* Pointer to first position of system buffer */
  int remain; /* Amount of bytes to complete the copy */
  int offset; /* Amount of already copied bytes */
  struct channel channels[NUM_CHANNELS];
  struct dyn_channel dyn_channels[NUM_CHANNELS];

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

#define KERNEL_ESP (DWord) &task[0].t.stack[KERNEL_STACK_SIZE]

/* Initializes initial process */
void init_task0(void);

void init_task_structs(void);

void init_queues(void);

static inline struct task_struct* list_head_to_task_struct(struct list_head *l) {
  return list_entry(l, struct task_struct, queue);
}

struct task_struct* current();

/** Returns the task struct corresponding to that pid, NULL_TSK if pid does NOT exist */
struct task_struct* pid_to_task_struct(int pid);

struct task_struct* get_new_task_struct();
void free_task_struct(struct task_struct* tsk);

/** Calls the scheduler, must be used in a interrupt, usually the clock */
void sched();

/** Selects next task that will use the CPU */
struct task_struct* sched_select_next();

/** Pause tsk task (keeping it in the run queue) */
void sched_pause(struct task_struct *tsk);

/** Restores tsk task */
void sched_continue(struct task_struct *tsk);

/** Blocks a task to a blocked queue */
void sched_block(struct task_struct *tsk, struct list_head *queue);

/** Unblocks a blocked task */
void sched_unblock(struct task_struct *tsk);

#endif  /* __SCHED_H__ */
