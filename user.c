#include <libc.h>

int __attribute__ ((__section__(".text.main"))) main(void) {
  int pid;
  write(1, "Aquesta entrega esta dedicada a Dennis Ritchie 1941 - 9 d'Octubre 2011 per la \
seva contribucio al llenguatge C i sistemes Unix, intimaments lligats a aquesta \
assignatura que mori envoltat de la seva familia sense resso mediatic.\n", 220);

//  runjp();

  while(1);
}
