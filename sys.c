/*
 * sys.c - Syscalls implementation
 */

#define SYSBUFF_SIZE 1024

#include <devices.h>
#include <utils.h>
#include <errno.h>
#include <sched.h>
#include <mm.h>
#include <mm_address.h>
#include <sem.h>
#include <stats.h>

unsigned int pid_counter = 0;

int getNewPid() {
  pid_counter++; // TODO Check better place to control pid TODO check if value is overflowed
  return pid_counter;
}

int check_fd(int fd, int mode) {
  if (fd != 1 || mode != WRITE_MODE) {
    return -1;
  } else {
    return 0;
  }
}

int check_buffer(void *buffer) {
    if (buffer == NULL) {
      return -1;
    } else {
      return 0;
    }
}

int sys_write(int fd, char *buffer, int size) {
  char sysbuff[SYSBUFF_SIZE];
  int chuck, remain, copied;
  int written;    
  
  if (check_fd(fd, WRITE_MODE) == -1) return -EBADF;
  if (check_buffer((void*) buffer) == -1) return -EFAULT;
  if (size < 0) return -EINVAL;
    
  copied = 0;
  remain = size;
  while (remain) {
    if (remain < SYSBUFF_SIZE) {
      chuck = remain;
    } else {
      chuck = SYSBUFF_SIZE;
    }
   
    copy_from_user(buffer + copied, sysbuff, chuck);
    switch (fd) {
      case(1):
	written = sys_write_console(sysbuff, chuck);
	/* By construction sys_write can NOT fail */
	remain -= written;
	copied += written;
      default:
	break;
    }
  }
  
  return copied;
}

int sys_fork(void) {
  int i, f;
  int lpag, src, dst;
  union task_union *child;
  
//   child = NULL;
//   for (i=0; i < NR_TASKS; i++) { //TODO enllaçar blocs lliures
//     if (task[i].t.task.pid == NULL_PID) {
//       child = &task[i].t;
//       break;
//     }
//   }
//   
//   /* There is no space to allocate the task_struct */
//   if (i == NR_TASKS) return -EAGAIN;
  
  child = (union task_union *)get_new_task_struct();
  if (child == (union task_union *)NULL_TSK) return -EAGAIN;

  /* Task_struct copy */
  copy_data(current(), &child->task, KERNEL_STACK_SIZE*4);    

  /* First of all we duplicate all the data pages in the parent space, just below its data pages */
  lpag = (L_USER_START>>12) + NUM_PAG_CODE + NUM_PAG_DATA;
  src = L_USER_START + NUM_PAG_CODE*PAGE_SIZE;
  dst = src + NUM_PAG_DATA*PAGE_SIZE; 
  for (i = 0; i < NUM_PAG_DATA; i++) {
    f = alloc_frame();
    if (f == -1) return -ENOMEM;
    
    set_ss_pag(lpag + i, f);                                        /* Assing logic page to parent */
    copy_data((void *)(src + i * PAGE_SIZE), (void *)(dst + i * PAGE_SIZE), PAGE_SIZE); /* Data copy data */
    del_ss_pag(lpag + i);                                           /* De-assing logic page */
    child->task.phpages[i] = f;                                       /* Assing physic frame to child */
  }
  
  /* TLB Flush */
  set_cr3();
  
  child->task.pid = getNewPid();
  child->task.quantum = current()->quantum;
  
  child->task.st.tics = 0;
  child->task.st.cs = 0;
  child->task.st.remaining_quantum = child->task.quantum;
    
  /* Modifies child eax value (fork returns 0 to the child)*/
  child->stack[KERNEL_STACK_SIZE-EAX_POS] = 0;
  
  list_add(&child->task.queue, &runqueue);
    
  return child->task.pid;
}

int sys_getpid(void) {
  struct task_struct *tsk;
  
  tsk = current();
  return tsk->pid;
}

int sys_nice(int quantum) {
  struct task_struct *tsk;
  int old_quantum;
  
  if (quantum <=0) return -EINVAL;
  
  tsk = current();
  
  old_quantum = tsk->quantum;
  tsk->quantum = quantum;
  
  return old_quantum;
}

int sys_sem_init(int n_sem, unsigned int value) {
  struct task_struct *tsk;
  
  if ((n_sem < 0) || (n_sem >= NR_SEM)) return -EINVAL;
  if (sems[n_sem].owner != FREE_SEM) return -EBUSY;
  if (value < 0) return -EINVAL;
  
  tsk = current();
  sems[n_sem].owner = tsk->pid;
  sems[n_sem].value = value;
  INIT_LIST_HEAD(&sems[n_sem].queue);
  
  return 0;
}

int zero_returner() {
  return 0;
}

int sys_sem_wait(int n_sem) {
  if (n_sem < 0 || n_sem >= NR_SEM) return -EINVAL;
  if (sems[n_sem].owner == FREE_SEM) return -EINVAL;
  if (current()->pid == 0) return -EPERM;
  
  if (sems[n_sem].value > 0) {
    sems[n_sem].value--;
  } else {
    /* Correct wait must return 0 */
    ((unsigned long *)current())[KERNEL_STACK_SIZE-EAX_POS] = 0; 
    
    sched_block(current(), &sems[n_sem].queue);
    sched_continue((void *)sched_select_next());
  }  
  return 0;
}

int sys_sem_signal(int n_sem) {
  if (n_sem < 0 || n_sem >= NR_SEM) return -EINVAL;
  if (sems[n_sem].owner == FREE_SEM) return -EINVAL;
  
  if (list_empty(&sems[n_sem].queue)) {
    sems[n_sem].value++;
  } else {
    sched_unblock(list_head_to_task_struct(list_first(&sems[n_sem].queue)));
  }
  return 0;
}

int sys_sem_destroy(int n_sem) {
  struct task_struct *tsk;
  
  if (n_sem < 0 || n_sem >= NR_SEM) return -EINVAL;
  if (sems[n_sem].owner == FREE_SEM) return -EINVAL;
  if (current()->pid != sems[n_sem].owner) return -EPERM;
  
  sems[n_sem].owner = FREE_SEM;
  
  while (!list_empty(&sems[n_sem].queue)) {
    tsk = list_head_to_task_struct(list_first(&sems[n_sem].queue));
    ((unsigned long *)tsk)[KERNEL_STACK_SIZE-EAX_POS] = -1;
    sched_unblock(list_head_to_task_struct(list_first(&sems[n_sem].queue)));
  }
    
  return 0;
}

void sys_exit(void) {
  struct task_struct *tsk;
  int lpag;
  int i;
  
  tsk = current();
  
  if (tsk->pid != 0) {
    lpag = (L_USER_START>>12) + NUM_PAG_CODE + NUM_PAG_DATA;
    for (i = 0; i < NUM_PAG_DATA; i++) {
      del_ss_pag(lpag + i);
      free_frame(tsk->phpages[i]);
    }
    set_cr3();
  
    /* NR_SEM will never be big, so this will be always cost-less */
    for (i = 0; i < NR_SEM; i++) {
      if (sems[i].owner == tsk->pid) {
	sys_sem_destroy(i);
      }
    }
  
    list_del(&tsk->queue);
    free_task_struct(tsk);
    
    sched_continue((void *)sched_select_next());
  }
}

int sys_get_stats(int pid, struct stats *st) {
  struct task_struct *tsk;
  
  /* Checks st parameter */  
  if (!access_ok(WRITE, st, sizeof(struct stats))) return -EFAULT;
  
  /* Checks pid parameter */
  if (pid < 0) return -EINVAL;
  tsk = pid_to_task_struct(pid);
  if (tsk == (struct task_struct *)NULL_TSK) return -ESRCH;

  if (tsk->state != TASK_RUNNING &&
      tsk->state != TASK_READY &&
      tsk->state != TASK_BLOCKED) return -ESRCH;
  
  return copy_to_user(&tsk->st, st, sizeof(struct stats)); 
}

int sys_ni_syscall(dvoid) {
  printk("Not implemented yet!\n");
  return -ENOSYS;
}

/** Not real sys_call, used only for debug (uses a valid sys_call_table entry) */
int sys_debug(void) {
  printk("Debug syscall!\n");
  return 0;
}

