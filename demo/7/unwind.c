#include <stdio.h>
#include <stdlib.h>

const char *binary;
int a  = 10010;
struct frame {
  struct frame *next; // push %rbp
  void *addr;         // call f (pushed retaddr)
};

void backtrace() {
  struct frame *f;
  char cmd[1024];
  extern char end;

  asm volatile ("movq %%rbp, %0" : "=g"(f));
  for (; ; f = f->next) {
    printf("%016lx  end  %016lx  ", (long)f->addr,(long)&end); fflush(stdout);
    sprintf(cmd, "addr2line -e %s %p", binary, f->addr);
    system(cmd);
  }
}

void bar() {
  int b = 1;
  printf(">>>=== b = %d  &b = %016lx\n",++b,&b);
  backtrace();
}

void foo() {
  int b = 2;
  printf(">>>=== b = %d  &b = %016lx\n",++b,&b);
  bar();
}

int main(int argc, char *argv[]) {
  binary = argv[0];
  foo();
}
