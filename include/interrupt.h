/*
 * interrupt.h - Definició de les diferents rutines de tractament d'exepcions
 */

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <types.h>

#define IDT_ENTRIES 256

#define KERNEL_LVL 0
#define USER_LVL 3

extern Gate idt[IDT_ENTRIES];
extern Register idtR;

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL);
void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL);

void setIdt();
void init_tics();

/* Exceptions */
void divide_error_routine();
void debug_routine();
void nm1_routine();
void breakpoint_routine();
void overflow_routine();
void bounds_check_routine();
void invalid_opcode_routine();
void device_not_available_routine();
void double_fault_routine();
void coprocessor_segment_overrun_routine();
void invalid_tss_routine();
void segment_not_present_routine();
void stack_exception_routine();
void general_protection_routine();
void page_fault_routine();
void intel_reserved_routine();
void floatin_point_error_routine();
void alignment_check_routine();

/* Interrupts */
void clock_routine();
void keyboard_routine();

#endif  /* __INTERRUPT_H__ */
