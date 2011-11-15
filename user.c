#include <libc.h>

int __attribute__ ((__section__(".text.main"))) main(void) {

  runjp();

  write(1, "\n\nAquesta entrega esta dedicada a Dennis Ritchie 1941 - 9 d'Octubre 2011 \n\
pare dels sistemes Unix i llenguatge C, fonament d'aquesta assignatura.", 145);
  while(1);
  
}
