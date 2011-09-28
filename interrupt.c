/*
 * interrupt.c -
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>
#include <entry.h>

#include "kernutil.h"

unsigned long tics = 0;
char tacs = 0;

/** Used in Clock_routine **/
//int secs = 0;         /** **/
//char sec = 0;         /** **/
//int min = 0;          /** **/
//char m_str[8];        /** **/
//char s_str[8];        /** **/
//char str[8];          /** **/
/** ************************/
  
Gate idt[IDT_ENTRIES];
Register    idtR;

#define CHAR_MAP_SIZE 98
char char_map[] = 
{
  '\0','\0','1','2','3','4','5','6',
  '7','8','9','0','\'','¡','\0','\0',
  'q','w','e','r','t','y','u','i',
  'o','p','`','+','\0','\0','a','s',
  'd','f','g','h','j','k','l','ñ',
  '\0','º','\0','ç','z','x','c','v',
  'b','n','m',',','.','-','\0','*',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','7',
  '8','9','-','4','5','6','+','1',
  '2','3','0','\0','\0','\0','<','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0'
};

/* Returns -1 if out of bounds or is a control
 * character (\0) */
char translate_key(int k) {
  char c;
  
  if (k > 0 && k < CHAR_MAP_SIZE) {
    c = char_map[k];
    if (c != '\0'){
	return c;
    }
  }
  return -1; 
}

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL) {
  /***********************************************************************/
  /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
  /* ***************************                                         */
  /* flags = x xx 0x110 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00;    /* P = 1, ndefD = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL) {
  /***********************************************************************/
  /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
  /* ********************                                                */
  /* flags = x xx 0x111 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);

  //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
  /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
     the system calls will be thread-safe. */
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setIdt() {
  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
    
  /* Privilege level extract from: 
   * http://www.rlz.cl/papers/buffer_overflow/p59-0x04.txt */
  setInterruptHandler(0, divide_error_handler, KERNEL_LVL);
  setInterruptHandler(1, debug_handler, KERNEL_LVL);
  setInterruptHandler(2, nm1_handler, KERNEL_LVL);
  setInterruptHandler(3, breakpoint_handler, USER_LVL);     /** User Level */
  setInterruptHandler(4, overflow_handler, USER_LVL);       /** User Level */
  setInterruptHandler(5, bounds_check_handler, USER_LVL);   /** User Level */
  setInterruptHandler(6, invalid_opcode_handler, KERNEL_LVL);
  setInterruptHandler(7, devive_not_available_handler, KERNEL_LVL);
  setInterruptHandler(8, double_fault_handler, KERNEL_LVL);
  setInterruptHandler(9, coprocessor_segment_overrun_handler, KERNEL_LVL);
  setInterruptHandler(10, invalid_tss_handler, KERNEL_LVL);
  setInterruptHandler(11, segment_not_present_handler, KERNEL_LVL);
  setInterruptHandler(12, stack_exception_handler, KERNEL_LVL);
  setInterruptHandler(13, general_protection_handler, KERNEL_LVL);
  setInterruptHandler(14, page_fault_handler, KERNEL_LVL);
  setInterruptHandler(15, intel_reserved_handler, KERNEL_LVL);
  setInterruptHandler(16, floatin_point_error_handler, KERNEL_LVL);
  setInterruptHandler(17, alignment_check_handler, KERNEL_LVL);

  /* Interrupts */
  setInterruptHandler(32, clock_handler, KERNEL_LVL);
  setInterruptHandler(33, keyboard_handler, KERNEL_LVL);

  /* System Calls */
  setTrapHandler(0x80, (void *) system_call, USER_LVL);
  
  set_idt_reg(&idtR);
}

void divide_error_routine() {
  printk("Exception found\n");
  while(1);
}

void debug_routine() {
  printk("Debug exception found\n");
  while(1);
}

void nm1_routine() {
  printk("NM1 exception found\n");
  while(1);
}
	
void breakpoint_routine() {
  printk("Breakpoint exception found\n");
  while(1);
}

void overflow_routine() {
  printk("Overflow exception found");
  while(1);
}

void bounds_check_routine() {
  printk("Bounds Check exception found");
  while(1);
}

void invalid_opcode_routine() {
  printk("Invalid Opcode exception found");
  while(1);
}

void device_not_available_routine() {
  printk("Device not avaliable exception found");
  while(1);
}

void double_fault_routine() {
  printk("Exception found");
  while(1);
}

void coprocessor_segment_overrun_routine() {
  printk("Coprocessor segment overrun exception found");
  while(1);
}

void invalid_tss_routine() {
  printk("Invalid tss exception found");
  while(1);
}

void segment_not_present_routine() {
  printk("Segment not present exception found");
  while(1);
}

void stack_exception_routine() {
  printk("Stack exception found");
  while(1);
}

void general_protection_routine() {
  printk("General protection Exception found");
  while(1);
}

void page_fault_routine() {
  printk("Page fault exception found");
  while(1);
}

void intel_reserved_routine() {
  printk("Intel reserved exception found");
  while(1);
}

void floatin_point_error_routine() {
  printk("Floatin point endefxception found");
  while(1);
}

void alignment_check_routine() {
  printk("Alignment check exception found");
  while(1);
}

// void clock_routine() { 
//   tics++;
//   
//   if (tacs == 0) {
//     tacs = 18;
//     secs++;
//     sec = secs % 60;
//     min = secs / 60;
// 
//     itoap(sec, s_str, 10, 2);
//     itoa(min, m_str, 10);
// 
//     strcat(str, m_str, ":");
//     strcat(str, str, s_str);
//     
//     printk_xyr(79, 0, str);
//   }
//   tacs--;
// 
// }

void clock_routine() {
  int secs;
  char sec, min;
  char m_str[8];
  char s_str[8];
  char str[8];

  sec = 0;
  min = 0;

  tics++;

  secs = tics / 18;
  sec = secs % 60;
  min = secs / 60;

  itoap(sec, s_str, 10, 2);
  itoa(min, m_str, 10);

  strcat(str, m_str, ":");
  strcat(str, str, s_str);

  printk_xyr(79, 0, str);

}



void keyboard_routine() {
  char k, c;
  char event;
    
  k = inb(KEYBOARD_PORT);
    
  /* Make/Break */
  event = (0x80 & k) == 0x80;
  /* Warning in break state first bit must be removed */
  
  if (event == KEY_MAKE) {
    c = translate_key(k);
    if (c != -1) {
      /* Removes Control string from screen if is there */
       printk_xyr(79, 24, "       ");
       printc_xy(79, 24, c);
    } else {
      printk_xyr(79, 24, "Control");
    }
  }
}


