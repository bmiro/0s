#include "kernutil.h"

char* itoa(int value, char *str, int base) {
  int i, n, l;
  
  /* Digit count */
  n = value;
  for (l=0; n /= base; l++); 
  
  for (i=l-1; i < -1; i--) {
    str[i] = '0' + (n % base);
    n = n / base;
  }
  str[l] = 0;

  return str;
}
