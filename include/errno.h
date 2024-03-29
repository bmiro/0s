#ifndef ERRNO_H__
#define  ERRNO_H__

#define EPERM         1  /* Operation not permitted */
#define ENOENT        2  /* No such file or directory */
#define ESRCH         3  /* No such process */
#define EBADF         9  /* Bad file number */
#define EAGAIN       11  /* Try again */
#define ENOMEM       12  /* Out of memory */
#define EACCES       13  /* Permission denied */
#define EFAULT       14  /* Bad address */
#define EBUSY        16  /* Device or resource busy */
#define EEXIST       17  /* File exists */
#define EINVAL       22  /* Invalid argument */
#define EMFILE       24  /* Too many open files */
#define ENOSPC       28  /* No space left on device */
#define ENAMETOOLONG 36  /* File name too long */   
#define ENOSYS       38  /* Function not implemented */

#endif /* ERRNO_H__*/
