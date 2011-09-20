#ifndef __KERNUTIL_H__
#define __KERNUTIL_H__

char* itoa(int value, char *str, int base);
/* Same as itoa but padding with 0 in left to achif a minlengh */
char* itoap(int value, char *str, int base, int minlengh);
char* strcat(char* dst, char* first, char* second);

#endif  /* __KERNUTIL_H__ */