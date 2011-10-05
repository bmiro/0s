#include <utils.h>
#include <types.h>
#include <mm_address.h>
#include <mm.h>

void copy_data(void *start, void *dest, int size) {
  DWord *p = start, *q = dest;
  Byte *p1, *q1;
  while(size > 4) {
    *q++ = *p++;
    size -= 4;
  }
  p1=(Byte*)p;
  q1=(Byte*)q;
  while(size > 0) {
    *q1++ = *p1++;
    size --;
  }
}

/* Copia de espacio de usuario a espacio de kernel, devuelve 0 si ok y -1 si error*/
int copy_from_user(void *start, void *dest, int size) {
  DWord *p = start, *q = dest;
  Byte *p1, *q1;
  while(size > 4) {
    *q++ = *p++;
    size -= 4;
  }
  p1=(Byte*)p;
  q1=(Byte*)q;
  while(size > 0) {
    *q1++ = *p1++;
    size --;
  }
  return 0;
}
/* Copia de espacio de kernel a espacio de usuario, devuelve 0 si ok y -1 si error*/
int copy_to_user(void *start, void *dest, int size) {
  DWord *p = start, *q = dest;
  Byte *p1, *q1;
  while(size > 4) {
    *q++ = *p++;
    size -= 4;
  }
  p1=(Byte*)p;
  q1=(Byte*)q;
  while(size > 0) {
    *q1++ = *p1++;
    size --;
  }
  return 0;
}

/* access_ok: Checks if a user space pointer is valid
 * @type:  Type of access: %VERIFY_READ or %VERIFY_WRITE. Note that
 *         %VERIFY_WRITE is a superset of %VERIFY_READ: if it is safe
 *         to write to a block, it is always safe to read from it
 * @addr:  User space pointer to start of block to check
 * @size:  Size of block to check
 * Returns true (nonzero) if the memory block may be valid,
 *         false (zero) if it is definitely invalid
 */
int access_ok(int type, const void *addr, unsigned long size) {
  int pag_addr;
  int page;
  
  /* Checks if block is in user-space logical memory range */
  if (((unsigned int)addr < L_USER_START) || 
      (unsigned int)(addr+size) > (L_USER_START + (NUM_PAG_CODE + NUM_PAG_DATA)*PAGE_SIZE)) {
    return 0;
  }
  
  if (type == READ) return 1; /* No need to check write access */
  
  /* Checks if pointer is in user data range */
  return ((unsigned int)addr > (L_USER_START + (NUM_PAG_CODE*PAGE_SIZE)));
 
//   /* Checks write access to all involved pages in this mem range */
//   for (pag_addr = addr; pag_addr <= addr+size; pag_addr += PAGE_SIZE) {
//    page = pag_addr>>12;
//    if (pagusr_table[page].bits.rw != 1) return 0; 
//   }
//  return 1;
}

