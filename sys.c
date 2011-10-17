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

unsigned int pid_counter = 1;

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
  
  child = NULL;
  for (i=0; i < NR_TASKS; i++) { //TODO enllaçar blocs lliures
    if (task[i].t.task.pid == NULL_PID) {
      child = &task[i].t;
      break;
    }
  }
  /* There is no space to allocate the task_struct */
  if (i == NR_TASKS) return -EAGAIN;

  /* Task_struct copy */
  copy_data(current(), &task[i].t.task, KERNEL_STACK_SIZE);    

  /* First of all we duplicate all the data pages in the parent space, just below its data pages */
  lpag = (L_USER_START>>12) + NUM_PAG_CODE + NUM_PAG_DATA;
  src = L_USER_START + NUM_PAG_CODE*PAGE_SIZE;
  dst = src + NUM_PAG_DATA*PAGE_SIZE; 
  for (i = 0; i < NUM_PAG_DATA; i++) { //TODO hem de mirar si totes les pagines de dades estan assignades??
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
  
  task->st.tics = 0;
  task->st.cs = 0;
  task->st.remaining_quantum = task->quantum;
    
  /* Modifies child eax value (fork returns 0 to the child)*/
  child->stack[KERNEL_STACK_SIZE-10] = 0;
  
  list_add(&child->task.queue, &runqueue);
  
  return child->task.pid;
}

void sys_exit(void) {
  struct task_struct *tsk;
  int lpag;
  
  tsk = current();
  
  if (tsk->pid != 0) {
    lpag = (L_USER_START>>12) + NUM_PAG_CODE + NUM_PAG_DATA;
    for (i = 0; i < NUM_PAG_DATA; i++) { //TODO hem de mirar si totes les pagines de dades estan assignades??
      del_ss_pag(lpag + i); // TODO Revisar si es necessari
      free_frame(tsk->task.phpages[i]);
    }
    set_cr3(); // TODO Revisar si es necessari
  
    //TODO alliberar semafors
  
    list_del(tsk->queue);
    tsk->pid = NULL_PID;
    
    sched_continue(sched_select_next());
  }
}

int sys_getpid(void) {
  struct task_struct *tsk;
  
  tsk = current();
  
  return tsk->pid;
}

int sys_sem_init(int n_sem, unsigned int value) {
  struct task_struct *tsk;
  
  if (n_sem < 0 || n_sem < NR_SEM) return -1;
  if (sems[n_sem].owner != FREE_SEM) return -1;
  
  tsk = current();
  sems[n_sem].owner = tsk.pid;
  sems[n_sem].value = value;
  INIT_LIST_HEAD(sems[n_sem].queue);
  
  return 0;
}

int sys_sem_wait(int n_sem) {
  if (n_sem < 0 || n_sem < NR_SEM) return -1;
  if (sems[n_sem].owner == FREE_SEM) return -1;
  
  if (sems[n_sem] <= 0) {
    sched_block(current(), &sems[n_sem].queue);
  } else {
    sems[n_sem].value--;
  }
  return 0;
}

int sys_sem_signal(int n_sem) {
  if (n_sem < 0 || n_sem < NR_SEM) return -1;
  if (sems[n_sem].owner == FREE_SEM) return -1;
  
  if (list_empty(sems[n_sem].queue)) {
    sems[n_sem].value++;
  } else {
    sched_unblock(list_first(sems[n_sem].queue));
  }
  return 0;
}

int sys_sem_destroy(int n_sem) { 
  struct task_struct tsk;
  tsk = current();
  
  if (n_sem < 0 || n_sem < NR_SEM) return -1;
  if (sems[n_sem].owner == FREE_SEM) return -1;
  if (tsk->pid != sems[n_sem].owner) return -1;
  
  if (list_empty(sems[n_sem].queue)) {
    sems[n_sem].owner = FREE_SEM;
  } else {
    //TODO avisar al sem_wait si es destruit i alliberar processos
  }	
  return 0;
}

int sys_get_stats(int pid, struct stat *st) {
  int i;
  struct task_struct *tsk;
  
  
  if (pid > pid_counter) return -1;
  
  tsk = getTS(pid);
  
  //TODO podem tenir processos que no siguin actius??, si es aixi contemplar-ho
  
  if (tsk == NULL_TSK) return -1;
  
  return copy_to_user(tsk->st, st, sizeof(stats));
  
}

int sys_ni_syscall(void) {
  printk_xyr(79, 20, "Not implemented yet!");
  return -ENOSYS;
}

