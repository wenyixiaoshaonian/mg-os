#include "co.h"
#include <stdlib.h>

#define LOCAL_MACHINE
#define STACK_SIZE 64*1024

#ifdef LOCAL_MACHINE
  #define debug(...) printf(__VA_ARGS__)
#else
  #define debug()
#endif
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
} ctx_t;

// ctx_t *context main_ctx = NULL;

enum co_status {
  CO_NEW = 1, // 新创建，还未执行过
  CO_RUNNING, // 已经执行过
  CO_WAITING, // 在 co_wait 上等待
  CO_DEAD,    // 已经结束，但还未释放资源
};

struct co {
  char *name;
  void (*func)(void *); // co_start 指定的入口地址和参数
  void *arg;

  enum co_status status;  // 协程的状态
  struct co *    waiter;  // 是否有其他协程在等待当前协程
  ctx_t        *context; // 寄存器现场
  void *stack;
  // uint8_t        stack[STACK_SIZE]; // 协程的堆栈
};

/*
 * %rdi: this, %rsi: cur_ctx, %rdx: new_ctx
 * save current context to parameter_1: cur_ctx, switch context to parameter_2: new_ctx
 */
void switch(ctx_t *ctx_t, ctx_t *ctx_t)
{
    __asm__ __volatile__ (
    "       movq %rsp, 0(%rsi)          \n"    // save stack pointer
    "       movq %rbp, 8(%rsi)          \n"    // save frame pointer
    "       movq (%rsp), %rax           \n"
    "       movq %rax, 16(%rsi)         \n"    // save pc pointer
    "       movq %rbx, 24(%rsi)         \n"    // save rbx, r12 - r15
    "       movq %r12, 32(%rsi)         \n"
    "       movq %r13, 40(%rsi)         \n"
    "       movq %r14, 48(%rsi)         \n"
    "       movq %r15, 56(%rsi)         \n"
    "       movq 56(%rdx), %r15         \n"
    "       movq 48(%rdx), %r14         \n"
    "       movq 40(%rdx), %r13         \n"    // restore rbx, r12 - r15
    "       movq 32(%rdx), %r12         \n"
    "       movq 24(%rdx), %rbx         \n"
    "       movq 8(%rdx), %rbp          \n"    // restore frame pointer 
    "       movq 0(%rdx), %rsp          \n"    // restore stack pointer
    "       movq 16(%rdx), %rax         \n"    // restore pc pointer
    "       movq %rax, (%rsp)           \n"    // push pc pointer in stack
    "       ret                           "
    );
}

struct co *co_start(const char *name, void (*func)(void *), void *arg) {

  if(!main_ctx) {
    main_ctx = (ctx_t *)malloc(ctx_t);
  }
  struct co *cur = (struct co *)malloc(sizeof(struct co));

  cur->name = name;
  cur->func = func;
  cur->arg = arg;

  cur->statu = CO_NEW;

  cur->stack = malloc(STACK_SIZE);

  cur->context = (ctx_t *)malloc(ctx_t);
  cur->context.rbp = stack;
  cur->context.rsp = stack + STACK_SIZE - (sizeof(void *)*2);

  cur->context.rip = func(cur->arg);

  return cur;
}

void co_wait(struct co *co) {
  while(1) {
    if (co->statu == CO_NEW) {
      // switch(main_ctx,co->context);
    }
    else if (co->statu == CO_RUNNING) {
      
    }
    else if (co->statu == CO_WAITING) {
      
    }
    else if (co->statu == CO_DEAD) {
      free(cur->stack);
      free(cur->co);
      printf(">>>=== bye......\n");
    }
  }
}

void co_yield() {

}
