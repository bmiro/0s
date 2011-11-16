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
#include <file.h>
#include <fat.h>

unsigned int pid_counter = 0;
char sysbuff[SYSBUFF_SIZE];


/****************************** Internal Funtions ******************************/
int getNewPid() {
  /* An unsigned int is a huge value for this educational OS,
  * we don't control overflow for simplicity */
  pid_counter++;
  return pid_counter;
}

int check_fd(int fd, int mode) {
  return (current()->channels[fd].mode & mode) == mode;
}

// /************************ Service interrupt routines ************************/
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

int sys_fork(void) {
  int i, j, f;
  int lpag, src, dst;
  union task_union *child;
  
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

    if (f == -1) { /* If it fails free the allocated frames  */
       for (j = i; j >= 0; j--) {
         free_frame(child->task.phpages[j]);
       }
       /* TLB Flush */
       set_cr3();
       return -ENOMEM;
    }
    
    set_ss_pag(lpag + i, f);      /* Assing logic page to parent */
    copy_data((void *)(src + i * PAGE_SIZE), (void *)(dst + i * PAGE_SIZE), PAGE_SIZE); /* Data copy data */
    del_ss_pag(lpag + i);        /* De-assing logic page */
    child->task.phpages[i] = f;  /* Assing physic frame to child */
  }
  
  /* TLB Flush */
  set_cr3();
  
  child->task.pid = getNewPid();
  child->task.quantum = current()->quantum;
  
  child->task.st.tics = 0;
  child->task.st.cs = 0;
  child->task.st.remaining_quantum = child->task.quantum;
  
  /* File descriptors hierachy */
  for (i = 0; i < NUM_CHANNELS && current->channels[i].mode != FREE_CHANNEL; i++) {
    child->task.channels[i].file = current->channels[i].file;
    child->task.channels[i].mode = current->channels[i].mode;
    child->task.channels[i].offset = current->channels[i].offset;
    child->task.channels[i].functions = current->channels[i].functions;
  }
  
  /* Modifies child eax value (fork returns 0 to the child)*/
  child->stack[KERNEL_STACK_SIZE-EAX_POS] = 0;
  
  list_add(&child->task.queue, &runqueue);
    
  return child->task.pid;
}

int sys_read(int fd, char *buffer, int size) {
    
  if (check_fd(fd, O_RDONLY) == -1) return -EBADF;
  if (!access_ok(WRITE, (void*) buffer, size)) return -EFAULT;
  if (size < 0) return -EINVAL;
  
  //return current()->channels[fd].functions->f_write(&c, buffer, size);

}

int sys_write(int fd, char *buffer, int size) {
  int copied, remain, chuck, written;
  
  if (check_fd(fd, O_WRONLY) == -1) return -EBADF;
  if (!access_ok(WRITE, (void*) buffer, size)) return -EFAULT;
  if (size < 0) return -EINVAL;
    
  copy_from_user(buffer, sysbuff, size);
  
  copied = 0;
  remain = size;
  while (remain) {
    if (remain < SYSBUFF_SIZE) {
      chuck = remain;
    } else {
      chuck = SYSBUFF_SIZE;
    }
   
    copy_from_user(buffer + copied, sysbuff, chuck);

    written = current()->channels[fd].functions->f_write(sysbuff, size);
    if (written < 0) return written;
    
    remain -= written;
    copied += written;
  }
  
  return copied;
  
}

// int sys_write(int fd, char *buffer, int size) {
//   char sysbuff[SYSBUFF_SIZE];
//   int chuck, remain, copied;
//   int written;    
//   
//   if (check_fd(fd, O_WRONLY) == -1) return -EBADF;
//   if (!access_ok(WRITE, (void*) buffer, size)) return -EFAULT;
//   if (size < 0) return -EINVAL;
//     
//   copied = 0;
//   remain = size;
//   while (remain) {
//     if (remain < SYSBUFF_SIZE) {
//       chuck = remain;
//     } else {
//       chuck = SYSBUFF_SIZE;
//     }
//    
//     copy_from_user(buffer + copied, sysbuff, chuck);
//     switch (fd) {
//       case(1):
//         written = sys_write_console(sysbuff, chuck);
//         /* By construction sys_write can NOT fail */
//         remain -= written;
//         copied += written;
//         break;
//       default:
//         break;
//     }
//   }
//   
//   return copied;
// }

int sys_open(const char *path, int flags) {
  int f;
  int fd;
  struct channel *channels;
  
  //TODO check flags
  
  
  channels = current()->channels;
  
  f = fat_find_path(path);
  
  if (!f) {
    ; //TODO CREATE FILE IF O_CREATE
  }
  
  fd = find_free_channel(channels);
  if (fd < 0) return -EMFILE;
  
  channels[fd].file = f;
  channels[fd].mode = (O_RDONLY | O_WRONLY) & flags; //TODO: Check if corret
  channels[fd].offset = 0;
  
  return fd;
}

int sys_close(int fd) {
  int error;
  
  if (check_fd(fd, O_WRONLY|O_RDONLY) == -1) return -EBADF;
  
  error = current()->channels[fd].functions->f_close(NULL); //TODO canviar el parametre
  if (error < 0) return error;
  
  current()->channels[fd].mode = FREE_CHANNEL;
  
  return 0;
}

int sys_getpid(void) {
  struct task_struct *tsk;
  
  tsk = current();
  return tsk->pid;
}

int sys_sem_init(int n_sem, unsigned int value) {  
  if ((n_sem < 0) || (n_sem >= NR_SEM)) return -EINVAL;
  if (sems[n_sem].owner != FREE_SEM) return -EBUSY;
  if (value < 0) return -EINVAL;
  
  sems[n_sem].owner = current()->pid;
  sems[n_sem].value = value;
  INIT_LIST_HEAD(&sems[n_sem].queue);
  
  return 0;
}

int sys_sem_wait(int n_sem) {
  if (n_sem < 0 || n_sem >= NR_SEM) return -EINVAL;
  if (sems[n_sem].owner == FREE_SEM) return -EINVAL;
  if (current()->pid == 0) return -EPERM;
  
  if (sems[n_sem].value > 0) {
    sems[n_sem].value--;
  } else {
    sched_block(current(), &sems[n_sem].queue);
    sched_continue((void *)sched_select_next());
  }  
  return 0;
}

int sys_sem_signal(int n_sem) {
  struct task_struct *tsk;
  if (n_sem < 0 || n_sem >= NR_SEM) return -EINVAL;
  if (sems[n_sem].owner == FREE_SEM) return -EINVAL;
  
  if (list_empty(&sems[n_sem].queue)) {
    sems[n_sem].value++;
  } else {
    /* Correct sem_wait must return 0 */
    tsk = list_head_to_task_struct(list_first(&sems[n_sem].queue)); 
    ((unsigned long *)tsk)[KERNEL_STACK_SIZE-EAX_POS] = 0;   
    sched_unblock(tsk);
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

int sys_nice(int quantum) {
  int old_quantum;
  
  if (quantum <=0) return -EINVAL;
    
  old_quantum = current()->quantum;
  current()->quantum = quantum;
  
  return old_quantum;
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

int sys_dup(int fd) {
  int new_fd;
  struct channel *channels;
  
  if (check_fd(fd, O_WRONLY|O_RDONLY) == -1) return -EBADF;
  
  channels = current()->channels;
  
  new_fd = find_free_channel(channels);
  if (new_fd < 0) return -EMFILE;
  
  channels[new_fd].file = channels[fd].file;
  channels[new_fd].mode = channels[fd].mode;
  channels[new_fd].offset = channels[fd].offset;
  channels[new_fd].functions = channels[fd].functions;

  return new_fd;
}

int sys_ni_syscall(void) {
  printk("Not implemented yet!\n");
  return -ENOSYS;
}

/** Not real sys_call, used only for debug (uses a valid sys_call_table entry) */
int sys_debug(void) {
  printk("Debug syscall!\n");
  return 0;
}

