#include <libc.h>

int __attribute__ ((__section__(".text.main"))) main(void) {
//   int pid;
//   int p;
    
//   int semset = 0;
//   int sv;
//   p = fork();
//   while(1) {
//     if (p == 0) {
//       sv = sem_wait(0);
//       if (sv == 0) {
// 	write(1, "0", 4);
//       }
//       write(1, "Fill!\n", 6);
//     } else {
//       if (semset) {
// 	sem_signal(0);
//       }
//       sem_init(0, 0);
//       semset = 1;
//       write(1, "Pare!\n", 6);
//     }
//   }
  runjp();
    
  
  write(1, "Aquesta entrega esta dedicada a Dennis Ritchie 1941 - 9 d'Octubre 2011 per la \
seva contribucio al llenguatge C i sistemes Unix, intimaments lligats a aquesta \
assignatura que mori envoltat de la seva familia sense resso mediatic.\n", 229);
  while(1);
}

