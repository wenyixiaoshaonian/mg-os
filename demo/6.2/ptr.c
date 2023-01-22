#include <stdio.h>

char big[1 << 30];
int main() {
  unsigned *p;
  p = (void *)main;
  printf("%x \n",*p);
  // *p = 1;
  while(1) {
    sleep(1000);
  }
  return 0;
}
