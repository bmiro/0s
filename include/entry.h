/*
 * entry.h - Definició del punt d'entrada de les crides al sistema
 */

#ifndef __ENTRY_H__
#define __ENTRY_H__

/* Sys calls ids (index in sys_call_table) */
#define SYS_EXIT_ID 		1
#define SYS_FORK_ID 		2
#define SYS_WRITE_ID 		4
#define SYS_GETPID_ID  		20
#define SYS_SEM_INIT_ID		21
#define SYS_SEM_WAIT_ID		22
#define SYS_SEM_SIGNAL_ID	23
#define SYS_SEM_DESTROY_ID	24
#define SYS_DEBUG_ID		32
#define SYS_NICE_ID		34
#define SYS_GET_STATS_ID	35

void divide_error_handler();
void debug_handler();
void nm1_handler();
void breakpoint_handler();
void overflow_handler();
void bounds_check_handler();
void invalid_opcode_handler();
void devive_not_available_handler();
void double_fault_handler();
void coprocessor_segment_overrun_handler();
void invalid_tss_handler();
void segment_not_present_handler();
void stack_exception_handler();
void general_protection_handler();
void page_fault_handler();
void intel_reserved_handler();
void floatin_point_error_handler();
void alignment_check_handler();

void clock_handler();
void keyboard_handler();

int system_call();

#endif  /* __ENTRY_H__ */
