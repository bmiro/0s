/*
 * sched.c - initializes struct for task 0
 */

#include <sched.h>
#include <mm.h>

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

struct protected_task_struct task[NR_TASKS]
  __attribute__((__section__(".data.task")));
    
char eoi_from_interrupt;
int roundtics;

struct list_head freequeue;

struct task_struct* current() {
  unsigned long sp;
  
  __asm__ __volatile__(
      "movl %%esp, %0\n"
      : "=a" (sp)
      :
  );
  
  /* 2 Because we use empty protected blocks to control stack overflows */
  return (struct task_struct*)(sp & CURRENT_TASK_MASK);
}

void init_queues(void) {
  INIT_LIST_HEAD(&runqueue);
  INIT_LIST_HEAD(&freequeue);
}

void init_task_structs(void) {
  int i, j;
  for (i = 1; i < NR_TASKS; i++) { /* Skips 0 used for system */
    list_add(&task[i].t.task.queue, &freequeue);
    for (j = 0; j < NUM_CHANNELS; j++) {
      task[i].t.task.channels[j].dynamic = FREE_CHANNEL;
    } 
  }
}
   
void init_task0(void) {
  int i;
  
  struct task_struct *ts;
  ts = &task[0].t.task;

  /* Initializes paging for the process 0 adress space */
  set_user_pages(ts);
  set_cr3();
  
  ts->pid = 0; /* Setting PID 0 */
  ts->quantum = DEFAULT_QUANTUM;
  ts->st.tics = 0;
  ts->st.cs = 0;
  ts->st.remaining_quantum = ts->quantum;
  list_add(&ts->queue, &runqueue);
  
  for (i = 0; i < NUM_CHANNELS; i++) {
    ts->channels[i].dynamic = FREE_CHANNEL;
  } 

  /* Sets defaults std in out err to keyboard console console */
  set_default_std_in_out_err();
  
  ts->offset = 0;
  ts->remain = 0;
  ts->buff = NULL;
  
  roundtics = ts->quantum;
}

void task_switch(union task_union *t) {
  int lpag, i;  
  
  /* Updates TSS to point stack of t */
  /* Also clears stack */
  tss.esp0 = (DWord) &(t->stack[KERNEL_STACK_SIZE]);
  
  /* Updates page table to make accessible the data and stack pages to user */
  lpag = (L_USER_START>>12)+NUM_PAG_CODE;
  for (i=0; i < NUM_PAG_DATA; i++) {
    set_ss_pag(lpag + i, t->task.phpages[i]);
  }
  set_cr3();
      
  /* Switch system stack of new task. */
  __asm__ __volatile__(
    "movl %0, %%esp\n" 
    :
    : "g" (&(t->stack[KERNEL_STACK_SIZE - RESTORE_SIZE]))
  );
 
  if (eoi_from_interrupt) {
    __asm__ __volatile__(
      "movb $0x20, %al\n \
       outb %al, $0x20"
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
     popl %ds\n \
     popl %es\n \
     popl %fs\n \
     popl %gs\n \
     iret"
  );
}

struct task_struct* pid_to_task_struct(int pid) {
  int i;
  
  /* TOOPTIMIZE */
  for (i = 0; i < NR_TASKS; i++) {
    if (task[i].t.task.pid == pid) {
      return &task[i].t.task;
    }
  }
  
  return (struct task_struct*) NULL_TSK;
}

struct task_struct* get_new_task_struct() {
  struct task_struct *tsk;
  
  if (list_empty(&freequeue)) return (struct task_struct*)NULL_TSK;
  
  tsk = list_head_to_task_struct(list_first(&freequeue));
  list_del(&tsk->queue);
  
  return tsk;
}

void free_task_struct(struct task_struct* tsk) {
  tsk->pid = NULL_PID;
  list_add(&tsk->queue, &freequeue);
}

void sched_update_status() {
  struct task_struct *tsk;
  
  roundtics--;
  tsk = current();
  tsk->st.tics++;
  tsk->st.remaining_quantum--;
}

/** Returns true if the scheduling algorism determines than the
    current process must left the CPU */
char sched_switch_needed() {
  return roundtics == 0;
}

struct task_struct* sched_select_next() {
  return list_head_to_task_struct(list_first(&runqueue));
}

void sched_pause(struct task_struct *tsk) {
  list_del(&tsk->queue);
  list_add_tail(&tsk->queue, &runqueue);
  tsk->state = TASK_READY;
}

void sched_continue(struct task_struct *tsk) {
  roundtics = tsk->quantum;
  tsk->st.cs++;
  tsk->st.remaining_quantum = tsk->quantum;
  tsk->state = TASK_RUNNING;

  task_switch((void *) tsk);
}
  
void sched_block(struct task_struct *tsk, struct list_head *queue) {
  list_del(&tsk->queue);
  list_add_tail(&tsk->queue, queue);
  tsk->state = TASK_BLOCKED;
}

void sched_unblock(struct task_struct *tsk) {
  list_del(&tsk->queue);
  list_add_tail(&tsk->queue, &runqueue);
  tsk->state = TASK_READY;
  
  tsk->remain = 0;
  tsk->offset = 0;
}

/** Sched function used ONLY in clock interrupt */
void sched() {
  eoi_from_interrupt = 1;
  
  sched_update_status();  
  if (sched_switch_needed()) {
    sched_pause(current());
    sched_continue(sched_select_next());
  } 
}


