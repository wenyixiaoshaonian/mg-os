#include <stdio.h>

int main() {
  unsigned *p;
  p = (void *)main;
  printf("%x \n",*p);
  *p = 1;
  return 0;
}
