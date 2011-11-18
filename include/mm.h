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

#define FRAME_FROM_PH_ADDR(x) (x>>12)
#define FRAME_TO_PH_ADDR(x) (x<<12)

/* It will be great if is bigger than BLOCK_SIZE in fat.h
 * and SYSBUFF_SIZE % BLOCK_SIZE == 0 */
#define SYSBUFF_SIZE 1024
char sysbuff[SYSBUFF_SIZE];

/* Bytemap to mark the free physical pages */
extern Byte phys_mem[TOTAL_PAGES];

int init_frames(void);
int alloc_frame(void);
void free_frame(unsigned int frame);
void set_user_pages(struct task_struct *task);
void del_user_pages(unsigned page);

/* PAGING */
/* Variables containing the page directory and the page table */

extern Descriptor  *gdt;

extern TSS         tss; 

void init_mm();
void set_ss_pag(unsigned page, unsigned frame);
void del_ss_pag(unsigned page);
void set_cr3();

void setGdt();

void setTSS();

#endif  /* __MM_H__ */
