#include <libc.h>

int __attribute__ ((__section__(".text.main"))) main(void) {

  char c = 'Q';
  char d = 'D';
  int p;
  
  int f;
  
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

  f = open("fitxer", O_CREAT|O_WRONLY);

  read(STDIN, &c, 1);
  write(f, &c, 1);
  
  close(f);
  
  f = open("fitxer", O_CREAT|O_RDONLY);
  read(f, &d, 1);
  write(STDOUT, &d, 1);
  
  close(f);
}
