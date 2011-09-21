#include <libc.h>

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
    /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
    /* Uncomment next line to call the initial routine for the test cases */
    /* runjp(); */

  //__asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); /* Generates a General protection Exception */

  write(1, "ABCDEFGHIJKLMNOPQRST", 20);
  
  while(1);
}
