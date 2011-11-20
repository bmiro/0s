/*
 * sys.c - Syscalls implementation
 */

#include <devices.h>
#include <file.h>
#include <utils.h>
#include <errno.h>
#include <sched.h>
#include <mm.h>
#include <mm_address.h>
#include <sem.h>
#include <stats.h>
#include <fat.h>

unsigned int pid_counter = 0;

/****************************** Internal Funtions ******************************/
int getNewPid() {
  /* An unsigned int is a huge value for this educational OS,
  * we don't control overflow for simplicity */
  pid_counter++;
  return pid_counter;
}

int bad_fd(int fd) {  
  if (fd < 0 || fd > NUM_CHANNELS) return 1;
  
  return current()->channels[fd].dyn_chars == FREE_CHANNEL;
}

int fd_access_ok(int fd, int mode) {
  int dchrs;
  
  if (bad_fd(fd)) return 0;
  
  dchrs = current()->channels[fd].dyn_chars;
  return (current()->dyn_channels[dchrs].mode & mode) == mode;
}

/************************ Service interrupt routines ************************/
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
  for (i = 0; i < NUM_CHANNELS; i++) {
    child->task.channels[i].file = current()->channels[i].file;
    child->task.channels[i].fops = current()->channels[i].fops;
    child->task.channels[i].dyn_chars = current()->channels[i].dyn_chars;
  }
  
  for (i = 0; i < NUM_CHANNELS; i++) {
    child->task.dyn_channels[i].mode = current()->dyn_channels[i].mode;
    child->task.dyn_channels[i].offset = current()->dyn_channels[i].offset;
  }
  
  /* Modifies child eax value (fork returns 0 to the child)*/
  child->stack[KERNEL_STACK_SIZE-EAX_POS] = 0;
  
  list_add(&child->task.queue, &runqueue);
    
  return child->task.pid;
}

int sys_read(int fd, char *buffer, int size) {
  int file;
  int dchars;
  int offset;
  int read;
  
  if (!fd_access_ok(fd, O_RDONLY)) return -EBADF;
  if (!access_ok(WRITE, (void *) buffer, size)) return -EFAULT;
  if (size < 0) return -EINVAL;
     
  file = current()->channels[fd].file;
  dchars = current()->channels[fd].dyn_chars;
  offset = current()->dyn_channels[dchars].offset;

  read = current()->channels[fd].fops->f_read(file, buffer, offset, size);
  current()->dyn_channels[dchars].offset += read;
  
  return read;
}

int sys_write(int fd, char *buffer, int size) {  
  int file;
  int dchars;
  int offset;
  int written;

  if (!fd_access_ok(fd, O_WRONLY)) return -EBADF;
  if (!access_ok(WRITE, (void*) buffer, size)) return -EFAULT;
  if (size < 0) return -EINVAL;
      
  file = current()->channels[fd].file;
  dchars = current()->channels[fd].dyn_chars;
  offset = current()->dyn_channels[dchars].offset;
  
  written = current()->channels[fd].fops->f_write(file, buffer, offset, size);
  current()->dyn_channels[dchars].offset += written;
  
  return written;
}

int sys_open(const char *path, int flags) {
  int error;
  int f;
  int fd, dchars;

  printk("OPEN");
  
  
  if (flags > 0x15 || flags < 0) return -EINVAL;
  if (!access_ok(READ, (void*) path, 1)) return -EFAULT;
    
  f = fat_find_path(path);
  
  if (f < 0) return f;
  
  fd = find_free_channel(current()->channels);
  dchars = find_free_dyn_channel(current()->dyn_channels);

  if (fd < 0 || dchars < 0) return -EMFILE;
      
  if (f == FILE_NOT_FOUND) {
    if ((flags & O_CREAT) == O_CREAT) {      
      f = fat_create(path, flags & O_RDWR, &dev_file);     
      if (f < 0) return -1;
    } else {
      /* File does not exist */
      return -ENOENT;
    }
  } else { 
    if ((flags & (O_EXCL|O_CREAT)) == (O_EXCL|O_CREAT)) return -EEXIST;
  }
      
  /* Flag check */
  if ((fs.root[f].mode & O_RDWR) != (flags & O_RDWR)) return -EACCES;
  
  current()->channels[fd].fops = fs.root[f].fops;
    
  if (current()->channels[fd].fops->f_open != NULL) {
    error = current()->channels[fd].fops->f_open(f);
    if (error < 0) return error;
  }

  current()->channels[fd].file = f;
  current()->channels[fd].dyn_chars = dchars;
  current()->dyn_channels[dchars].mode = flags & O_RDWR;
  current()->dyn_channels[dchars].offset = 0;
    
  return fd;
}

int sys_close(int fd) {
  int error;
  int duped;
  int i;

  if (bad_fd(fd)) return -EBADF;
  
  duped = 0;
  for (i = 0; i < NUM_CHANNELS; i++) {
    if ((current()->channels[i].dyn_chars == current()->channels[fd].dyn_chars) && (i != fd)) {
      duped = 1;
      break;
    }
  }
  
  if (current()->channels[fd].fops->f_close != NULL) {
    error = current()->channels[fd].fops->f_close(current()->channels[fd].file);
    if (error < 0) return error;
  }
  
  if (!duped) {
    current()->dyn_channels[current()->channels[fd].dyn_chars].mode = FREE_CHANNEL;
  }
  current()->channels[fd].dyn_chars = FREE_CHANNEL;  
  return 0;
}

int sys_unlink(const char *path) {
  int f;
  int error;
  struct file_operations *fops;
  
  if (!access_ok(READ, (void*) path, 1)) return -EFAULT;
      
  f = fat_find_path(path);
  if (f < 0) return f;
  if (f == FILE_NOT_FOUND) return -ENOENT;
  
  if (fat_get_opens(f) > 0) return -EBUSY;
        
 fat_get_fops(f, &fops);
  if (fops->f_unlink != NULL) {
    error = fops->f_unlink(f);
    if (error < 0) return error;
  }
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

  if (bad_fd(fd)) return -EBADF;
  
  new_fd = find_free_channel(current()->channels);
  if (new_fd < 0) return -EMFILE;
  
  current()->channels[new_fd].file = current()->channels[fd].file;
  current()->channels[new_fd].fops = current()->channels[fd].fops;
  current()->channels[new_fd].dyn_chars = current()->channels[fd].dyn_chars;
  
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

