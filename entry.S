/*
 * entry.S - Entry point to system mode from user mode
 */

#include <asm.h>
#include <segment.h>


/**************************************************/
/**** Save & Restore ******************************/
/**                                              **/
/** When we change to privilege level 0 (kernel) **/
/** (through an interrupt, a system call, an     **/
/** exception ...) we must save the state of the **/
/** currently running task (save).               **/
/**                                              **/
/** Stack layout in 'systemCall':                **/
/**                                              **/
/**   0(%esp) - %ebx    \                        **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %edx     |                       **/
/**   C(%esp) - %esi     | Register saved        **/
/**  10(%esp) - %edi     |  by 'save'            **/
/**  14(%esp) - %ebp     |                       **/
/**  18(%esp) - %eax     |                       **/
/**  1C(%esp) - %ds      |                       **/
/**  20(%esp) - %es      |                       **/
/**  24(%esp) - %fs      |                       **/
/**  28(%esp) - %gs     /                        **/
/**  2C(%esp) - %eip    \                        **/
/**  30(%esp) - %cs      |                       **/
/**  34(%esp) - %eflags  |  Return context saved **/
/**  38(%esp) - %oldesp  |   by the processor.   **/
/**  3C(%esp) - %oldss  /                        **/
/**                                              **/
/**************************************************/

#define SAVE_ALL \
      pushl %gs; \
      pushl %fs; \
      pushl %es; \
      pushl %ds; \
      pushl %eax; \
      pushl %ebp; \
      pushl %edi; \
      pushl %esi; \
      pushl %edx; \
      pushl %ecx; \
      pushl %ebx; \
      movl $__KERNEL_DS, %edx;    \
      movl %edx, %ds;           \
      movl %edx, %es

#define RESTORE_ALL \
	movl %edx, %edx

	TODO HERE implementar restauracio de registres


/* Interrupt & Exception handlers */
ENTRY(divide_error_handler)
	SAVE_ALL
	call divide_error_routine
	RESTORE_ALL
	/* Delete param if needed */
	iret




/*
debug_handler:
	SAVE_ALL
	call debug_routine
	RESTORE_ALL
	iret

nm1_handler:
	SAVE_ALL
	call nm1_routine
	RESTORE_ALL
	iret

breakpoint_handler:
	SAVE_ALL
	call breakpoint_routine
	RESTORE_ALL
	iret

overflow_handler:
	SAVE_ALL
	call overflow_handler
	RESTORE_ALL
	iret

bounds_check_handler:
	SAVE_ALL
	call 
	RESTORE_ALL
	iret


invalid_opcode_handler:
	SAVE_ALL
	call 
	RESTORE_ALL
	iret

device_not_available_handler:
	SAVE_ALL
	call 
	RESTORE_ALL
	iret

double_fault_handler:
	SAVE_ALL
	call 
	RESTORE_ALL
	iret

coprocessor_segment_overrun_handler:
	SAVE_ALL
	call 
	RESTORE_ALL
	iret

invalid_tss_handler:
	SAVE_ALL
	call 
	RESTORE_ALL
	iret

segment_not_present_handler:
	SAVE_ALL
	call 
	RESTORE_ALL
	iret

stack_exception_handler:
	SAVE_ALL
	call 
	RESTORE_ALL
	iret

general_protection_handler:
	SAVE_ALL
	call 
	RESTORE_ALL
	iret

page_fault_handler:
	SAVE_ALL
	call 
	RESTORE_ALL
	iret

intel_reserved_handler:
	SAVE_ALL
	call 
	RESTORE_ALL
	iret

floatin_point_error_handler:
	SAVE_ALL
	call 
	RESTORE_ALL
	iret

alignment_check_handler:
	SAVE_ALL
	call 
	RESTORE_ALL
	iret
*/
