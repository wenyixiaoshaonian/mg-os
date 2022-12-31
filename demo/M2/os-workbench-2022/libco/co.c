#include "co.h"
#include <stdlib.h>
#include <stdio.h>

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

enum co_status {
  CO_NEW = 1, // 新创建，还未执行过
  CO_RUNNING, // 已经执行过
  CO_WAITING, // 在 co_wait 上等待
  CO_DEAD,    // 已经结束，但还未释放资源
};

struct co {
  const char *name;
  void (*func)(void *); // co_start 指定的入口地址和参数
  void *arg;

  enum co_status status;  // 协程的状态
  struct co *    waiter;  // 是否有其他协程在等待当前协程
  ctx_t        *context; // 寄存器现场
  void *stack;
  // uint8_t        stack[STACK_SIZE]; // 协程的堆栈
};

struct co_list {
  struct co *co;
  struct co_list *next;
};

ctx_t *main_ctx = NULL;
struct co_list *list = NULL;
struct co_list *cur_list = NULL;
/*
 * %rdi: this, %rsi: cur_ctx, %rdx: new_ctx
 * save current context to parameter_1: cur_ctx, switch context to parameter_2: new_ctx
 */
void _switch(ctx_t *cur_ctx, ctx_t *new_ctx)
{
#ifdef __x86_64__
    __asm__ __volatile__ (
    "       movq %rsp, 0(%rdi)          \n"    // save stack pointer
    "       movq %rbp, 8(%rdi)          \n"    // save frame pointer
    "       movq (%rsp), %rax           \n"
    "       movq %rax, 16(%rdi)         \n"    // save pc pointer
    "       movq %rbx, 24(%rdi)         \n"    // save rbx, r12 - r15
    "       movq %r12, 32(%rdi)         \n"
    "       movq %r13, 40(%rdi)         \n"
    "       movq %r14, 48(%rdi)         \n"
    "       movq %r15, 56(%rdi)         \n"
    "       movq 56(%rsi), %r15         \n"
    "       movq 48(%rsi), %r14         \n"
    "       movq 40(%rsi), %r13         \n"    // restore rbx, r12 - r15
    "       movq 32(%rsi), %r12         \n"
    "       movq 24(%rsi), %rbx         \n"
    "       movq 8(%rsi), %rbp          \n"    // restore frame pointer 
    "       movq 0(%rsi), %rsp          \n"    // restore stack pointer
    "       movq 16(%rsi), %rax         \n"    // restore pc pointer
    "       movq %rax, (%rsp)           \n"    // push pc pointer in stack
    "       ret                           "
    );
#elif __i386__
    // __asm__ __volatile__ (
    // "       movl %esp, 0(%esi)          \n"    // save stack pointer
    // "       movl %ebp, 4(%esi)          \n"    // save frame pointer
    // "       movl (%esp), %eax           \n"
    // "       movl %eax, 8(%esi)         \n"    // save pc pointer
    // "       movl %ebx, 12(%esi)         \n"    // save rbx, r12 - r15
    // "       movl %r12d, 16(%esi)         \n"
    // "       movl %r13d, 20(%esi)         \n"
    // "       movl %r14d, 24(%esi)         \n"
    // "       movl %r15d, 28(%esi)         \n"
    // "       movl 28(%edx), %r15d         \n"
    // "       movl 24(%edx), %r14d         \n"
    // "       movl 20(%edx), %r13d         \n"    // restore rbx, r12 - r15
    // "       movl 16(%edx), %r12d         \n"
    // "       movl 12(%edx), %ebx         \n"
    // "       movl 4(%edx), %ebp          \n"    // restore frame pointer 
    // "       movl 0(%edx), %esp          \n"    // restore stack pointer
    // "       movl 8(%edx), %eax         \n"    // restore pc pointer
    // "       movl %eax, (%esp)           \n"    // push pc pointer in stack
    // "       ret                           "
    // );
#endif
}

void _exec() {
  printf(">>>=== hello......\n");
}
struct co *co_start(const char *name, void (*func)(void *), void *arg) {
  if(!main_ctx) {
    main_ctx = (ctx_t *)malloc(sizeof(ctx_t));
  }
  struct co *cur = (struct co *)malloc(sizeof(struct co));

  cur->name = name;
  cur->func = func;
  cur->arg = arg;

  cur->status = CO_NEW;

  cur->stack = malloc(STACK_SIZE);

  cur->context = (ctx_t *)malloc(sizeof(ctx_t));
#ifdef __x86_64__  
  cur->context->rbp = cur->stack;
  cur->context->rsp = cur->stack + STACK_SIZE - (sizeof(void *)*2);

  cur->context->rip = (void *)_exec;
#elif __i386__
  cur->context->ebp = cur->stack;
  cur->context->esp = cur->stack + STACK_SIZE - (sizeof(void *)*2);

  cur->context->eip = (void *)_exec;
#endif
  //add co into list
  if(!list) {
    list = (struct co_list *)malloc(sizeof(struct co_list));
    list->co = cur;
    list->next = NULL;
    list = cur_list;
  } else {
    struct co_list *clist = (struct co_list *)malloc(sizeof(struct co_list));
    clist->co = cur;
    cur_list->next = clist;
    cur_list = clist;
    cur_list->next = NULL;
  }
  return cur;
}

void co_wait(struct co *co) {
  while(1) {
    if (co->status == CO_NEW) {
      printf("main_ctx %p  co->context %p \n",main_ctx,co->context);
      _switch(main_ctx,co->context);
    }
    else if (co->status == CO_RUNNING) {
      
    }
    else if (co->status == CO_WAITING) {
      
    }
    else if (co->status == CO_DEAD) {
      free(co->stack);
      free(co);
      printf(">>>=== bye......\n");
      return;
    }
  }
}

void co_yield() {

}
