#include <stdio.h>
#include <unistd.h>

#include "hanoi-nr.c"

#define REGS_FOREACH(_)  _(X) _(Y)
#define OUTS_FOREACH(_)  _(A) _(B) _(C) _(D) _(E) _(F) _(G)
#define RUN_LOGIC        X1 = !X && Y; \
                         Y1 = !X && !Y; \
                         A  = (!X && !Y) || (X && !Y); \
                         B  = 1; \
                         C  = (!X && !Y) || (!X && Y); \
                         D  = (!X && !Y) || (X && !Y); \
                         E  = (!X && !Y) || (X && !Y); \
                         F  = (!X && !Y); \
                         G  = (X && !Y); 

#define DEFINE(X)   static int X, X##1;
#define UPDATE(X)   X = X##1;
//#X是用于将变量名转换为字符串的运算符
#define PRINT(X)    printf(#X " = %d; ", X);

int main() {
  // REGS_FOREACH(DEFINE);
  // OUTS_FOREACH(DEFINE);
  // while (1) { // clock
  //   RUN_LOGIC;
  //   OUTS_FOREACH(PRINT);
  //   REGS_FOREACH(UPDATE);
  //   putchar('\n');
  //   fflush(stdout);
  //   sleep(1);
  // }
  hanoi(3,'A','B','C');
}
