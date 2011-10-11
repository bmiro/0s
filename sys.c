/*
 * sys.c - Syscalls implementation
 */

#define SYSBUFF_SIZE 1024

#include <devices.h>
#include <utils.h>
#include <errno.h>
#include <sched.h>

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
  int i;
  struct task_struct *parent_tsk;
  struct task_struct *children_tsk;
  
  children_tsk = NULL;
  for (i=0; i < NR_TASKS; i++) {
    if (task[i].t.task.pid == NULL_PID) {
      children_tsk = task[i].t.task;
      break;
    }
  }
  /* There is no space to allocate the task_struct */
  if (i == NR_TASKS) return -EAGAIN;
  
  /* Task_struct copy */
  tsk = current();
  copy_data(tsk, &task[i].t.task, KERNEL_STACK_SIZE);
  
  /* Switch to child pagetable */
  //TODO
  
  for (i=NUM_PAG_CODE; i < NUM_PAG_DATA; i++) {
    if (parent_tsk->phpages[i] != NULL) {
      f = alloc_frame();
      set_ss_pag( ,f);
      //TODO verificar que ha alocat el frame
      copy_data(FRAME_TO_PH_ADDR(parent_tsk.phpages[i]), FRAME_TO_PH_ADDR(f), PAGE_SIZE);
      children_tsk->phpages[i] = f;
    } else {
      children_tsk->phpages[i] = NULL;
    }
    
    
  
  
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

