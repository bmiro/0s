#include <libc.h>

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
    /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
    /* Uncomment next line to call the initial routine for the test cases */
    /* runjp(); */

	TODO HERE desomentar linia del mov i implementar excepcio de proteccio de memoria que sempre va be

	int i, j, k;

	i = 1;
	j = 1;
	k = i+j;
	i = i - j;
	k = k / i;

  //while(1);
}
