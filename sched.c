/*
 * sched.c - initializes struct for task 0
 */

#include <sched.h>

#include <mm.h>

struct protected_task_struct task[NR_TASKS]
  __attribute__((__section__(".data.task")));

#define CURRENT_TASK_MASK 0xFFFFF000

#define RESTORE_SIZE 16
/* Restore Size for system stack
16 ---------- KERNEL_STACK_SIZE-16 <- ESP
   | ebx    |
15 ---------- KERNEL_STACK_SIZE-15
   | ecx    |
14 ---------- KERNEL_STACK_SIZE-14
   | edx    |
13 ---------- KERNEL_STACK_SIZE-13
   | esi    |
12 ---------- KERNEL_STACK_SIZE-12
   | edi    |
11 ---------- KERNEL_STACK_SIZE-11
   | ebp    |
10 ---------- KERNEL_STACK_SIZE-10
   | eax    |
09 ---------- KERNEL_STACK_SIZE-9
   | ds     |
08 ---------- KERNEL_STACK_SIZE-8
   | es     |
07 ---------- KERNEL_STACK_SIZE-7
   | fs     |
06 ---------- KERNEL_STACK_SIZE-6
   | gs     |
05 ---------- KERNEL_STACK_SIZE-5
   | eip    | 
04 ---------- KERNEL_STACK_SIZE-4
   | cs     | 
03 ---------- KERNEL_STACK_SIZE-3
   | eflags | 
02 ---------- KERNEL_STACK_SIZE-2
   | esp    |
01 ---------- KERNEL_STACK_SIZE-1
   | ss     |
00 ---------- KERNEL_STACK_SIZE
*/

char eoi_from_interrupt;

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
  struct task_struct *oldTsk;

  /* Updates TSS to point stack of t */
  /* Also clears stack */
  tss.esp0 = (DWord) &(t->stack[KERNEL_STACK_SIZE - RESTORE_SIZE]);
  
  /* Updates page table to make accessible the data and stack pages to user */
  lpag = (L_USER_START>>12)+NUM_PAG_CODE;
  for (i=0; i < NUM_PAG_DATA; i++) {
    set_ss_pag(lpag + i, t->task.phpages[i]);
  }
  set_cr3();
  
  oldTsk = current();
  oldTsk->state = TASK_READY;
  t->task.state = TASK_RUNNING;
  
  /* Switch system stack of new task. */
  __asm__ __volatile__(
    "movl %0, %%esp\n" 
    :
    : "g" (tss.esp0)
  );
 
  if (eoi_from_interrupt) {
    __asm__ __volatile__(
      "movb $0x20, %al\n"
      "outb %al, $0x20"
    );
    eoi_from_interrupt = 0;
  }
  
  /* Restore registers */
  __asm__ __volatile__(
    "popl %ebx\n \
     popl %ecx\n \
     popl %edx\n \
     popl %esi\n \
     popl %edi\n \
     popl %ebp\n \
     popl %eax\n \
     popl %ds\n	 \
     popl %es\n \
     popl %fs\n \
     popl %gs\n \
     iret"
  );
  
}
