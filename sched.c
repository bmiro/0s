/*
 * sched.c - initializes struct for task 0
 */

#include <sched.h>
#include <list.h>
#include <mm.h>

struct protected_task_struct task[NR_TASKS]
  __attribute__((__section__(".data.task")));

#define CURRENT_TASK_MASK 0xFFFFF000

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

void init_task_structs(void) {
  int i;
  for (i=0; i < NR_TASKS; i++) {
    task[i].t.task.pid = NULL_PID;
  }
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

void task_switch(union task_union *t) {
  int lpag, i;
  /* Updates TSS to point stack of t */
  tss.esp0 = &t->stack[KERNEL_STACK_SIZE];
  
  /* Updates page table to make accessible the data and stack pages to user */
  lpag = (L_USER_START>>12)+NUM_PAG_CODE;
  for (i=0; i < NUM_PAG_DATA; i++) {
    set_ss_page(lpag + i, t->task.phpages[i]);
  }
  set_cr3();
  
  t->task.state = TASK_RUN;
  
  /* Switch system stack of new task. */
  __asm__ __volatile__(
    "movl %0, %%esp\n" 
    :
    : "g" (tss.esp0)
  );

  
  /* Restore registers */


Actualitzar la taula de pàgines perquè les pàgines de dades+pila d’usuari de t siguin accessibles.

Canviar a la pila de sistema del nou procés Restaurar els registres.

Cal fer EOI? (penseu si cal i on s’ha de fer)

IRET (penseu perquè fem iret i no ret)

  
}