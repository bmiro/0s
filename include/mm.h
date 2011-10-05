/*
 * mm.h - Capçalera del mòdul de gestió de memòria
 */

#ifndef __MM_H__
#define __MM_H__

#include <types.h>
#include <mm_address.h>
#include <sched.h>

 
#define FREE_FRAME 0
#define USED_FRAME 1
/* Bytemap to mark the free physical pages */
extern Byte phys_mem[TOTAL_PAGES];

int init_frames( void );
int alloc_frame( void );
void free_frame( unsigned int frame );
void set_user_pages( struct task_struct *task );

/* PAGING */
/* Variables containing the page directory and the page table */

extern Descriptor  *gdt;

extern TSS         tss; 

void init_mm();
void set_ss_pag(unsigned page,unsigned frame);
void set_cr3();

void setGdt();

void setTSS();

#endif  /* __MM_H__ */
