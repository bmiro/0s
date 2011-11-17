#include <libc.h>

int __attribute__ ((__section__(".text.main"))) main(void) {

  char c = 'Q';
  char d = 'D';
  int p;
  //runjp();

  //write(1, "\n\nAquesta entrega esta dedicada a Dennis Ritchie 1941 - 9 d'Octubre 2011 \n\
pare dels sistemes Unix i llenguatge C, fonament d'aquesta assignatura.", 145);
  //while(1);
  
  p = fork();
  
  if (p == 0) {
    while(1) {
      read(0, &c, 1);
      write(1, &c, 1);
    }
  }
  
  while(1) {
    //write(1, "FATHER", 6);
  }

}
