#include <libc.h>

int __attribute__ ((__section__(".text.main"))) main(void) {
  
  runjp();
  
  while(1);
}
