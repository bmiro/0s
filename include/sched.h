/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024

struct task_struct {
  int dummy;
  // SUBSTITUIU EL CAMP dummy PER QUE CONSIDEREU QUE HA DE TENIR EL TASK_STRUCT
};

union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};

struct protected_task_struct {
  unsigned long task_protect[KERNEL_STACK_SIZE];  /* This field protects the different task_structs, so any acces to this field will generate a PAGE FAULT exeption */
  union task_union t;
};

extern struct protected_task_struct task[NR_TASKS];

#define KERNEL_ESP       (DWord) &task[0].t.stack[KERNEL_STACK_SIZE]

/* Inicialitza les dades del proces inicial */
void init_task0(void);

#endif  /* __SCHED_H__ */
