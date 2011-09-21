#include "kernutil.h"

char* itoa(int value, char *str, int base) {
  int i, n, l;
  
  /* Digit count */
  n = value;
  for (l=1; n /= base; l++);
  
  n = value;
  for (i=l-1; i > -1; i--) {
    str[i] = '0' + (n % base);
    n = n / base;
  }
  str[l] = 0;

  return str;
}

/* Same as itoa but padding with 0 in left to achif a minlengh */
char* itoap(int value, char *str, int base, int minlengh) {
  int i, n, l;
  int p, q;
  
  /* Digit count */
  n = value;
  for (l=1; n /= base; l++);
  
  p = minlengh - l;
  if (p < 0) {
    return itoa(value, str, base);
  }
  
  /* Real number */
  n = value;
  for (i=l-1+p; i > -1; i--) {
    str[i] = '0' + (n % base);
    n = n / base;
  }  

  /* Padding with left-0s */
  for (q=0; q < p; q++) {
    str[q] = '0';
  }

  str[l] = 0;

  return str;
  
}

char* strcat(char* dst, char* first, char* second) {
  int i, j;
  
  for(i=0; first[i]; i++) {
    dst[i] = first[i];
  }
  
  for (j=0; second[j]; j++) {
    dst[i] = second[j];
    i++;
  }
  dst[i] = '\0';
  
  return dst;
}
