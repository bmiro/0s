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
  for (i=0; i < NR_TASKS; i++) {
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
  dst = src + NUM_PAG_CODE*PAGE_SIZE; 
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
  child->task.state = TASK_READY;
  /* Modifies child eax value (fork returns 0 to the child)*/
  child->stack[KERNEL_STACK_SIZE-10] = 0;
  
  list_add(&child->task.queue, &runqueue);
  
  return child->task.pid;
}

int sys_getpid(void) {
  struct task_struct *tsk;
  
  tsk = current();
  
  return tsk->pid;
}

int sys_ni_syscall(void) {
  printk_xyr(79, 20, "Not implemented yet!");
  return -ENOSYS;
}

