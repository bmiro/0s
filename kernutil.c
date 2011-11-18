#include "kernutil.h"

char* itoa(int value, char *str, int base) {
  int i, l;
  int v;
  
  /* Digit count */
  v = value;
  for (l=1; v /= base; l++);
  
  v = value;
  for (i=l-1; i > -1; i--) {
    str[i] = '0' + (v % base);
    v /= base;
  }
  str[l] = 0;

  return str;
}

/* Same as itoa but padding with 0 in left to achive a minlengh */
char* itoap(int value, char *str, int base, int minlengh) {
  int i, n, l;
  int v;
  
  /* Digit count */
  v = value;
  for (l=1; v /= base; l++);
  
  n = minlengh - l;
  if (n <= 0) {
    return itoa(value, str, base);
  }
  
  v = value;
  for (i=l-1+n; i > -1; i--) {
    str[i] = '0';
    if (i >= n) {
      str[i] += (v % base);
      v /= base;
    }
  }
  str[l+n] = 0;

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

int strcmp(char* s1, char* s2) {
  int i;
  
  for (i = 0; s1 != '\0'; i++) {
    if (s1[i] - s2[i]) break;
  }
  
  return s1[i] - s2[i];
  
}

