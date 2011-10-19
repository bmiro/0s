#include <libc.h>

int __attribute__ ((__section__(".text.main"))) main(void) {
  int pid;
  int p;
    
  write(1, "Aquesta entrega esta dedicada a Dennis Ritchie 1941 - 9 d'Octubre 2011 per la \
seva contribucio al llenguatge C i sistemes Unix, intimaments lligats a aquesta \
assignatura que mori envoltat de la seva familia sense resso mediatic.\n", 229);

  p = fork();

  while(1) {
    if (p == 0) {
      write(1, "Fill!\n", 5);
    } else {
      write(1, "Pare!\n", 6);
    }
  }
  //runjp();
}
