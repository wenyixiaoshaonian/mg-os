#include "co.h"
#include <stdlib.h>

#define STACK_SIZE 16*1024
#ifdef LOCAL_MACHINE
  #define debug(...) printf(__VA_ARGS__)
#else
  #define debug()
#endif

enum co_status {
  CO_NEW = 1, // 新创建，还未执行过
  CO_RUNNING, // 已经执行过
  CO_WAITING, // 在 co_wait 上等待
  CO_DEAD,    // 已经结束，但还未释放资源
};

typedef struct {
#ifdef __x86_64__
    void *rsp;
    void *rbp;
    void *rip;
    void *rbx;
    void *r12;
    void *r13;
    void *r14;
    void *r15;
#elif __i386__
    void *esp;
    void *ebp;
    void *eip;
    void *ebx;
    void *r12d;
    void *r13d;
    void *r14d;
    void *r15d;
#endif
}reg_info;

struct co {
  char *name;
  int status;
  void (*func)(void*);
  void* arg;
  reg_info reg;
  uint8_t stack[STACK_SIZE];
};

struct co *co_start(const char *name, void (*func)(void *), void *arg) {
  struct co *ret = (struct co*)malloc(sizeof(struct co));
  ret->stack = malloc(sizeof(uint8_t) * STACK_SIZE);
  ret->reg = (reg_info*)malloc(sizeof(reg_info));

  ret->name = name;
  ret->func = func;
  ret->arg = arg;
  return ret;
}

void co_wait(struct co *co) {

}

void co_yield() {

}
