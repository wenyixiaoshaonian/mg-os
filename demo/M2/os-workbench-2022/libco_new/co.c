#include "co.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
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
#endif
}reg_info;

struct co {
  const char *name;
  int status;
  void (*func)(void*);
  void* arg;
  reg_info* reg;
  uint8_t* stack;
  struct co* last;
  struct co* next;
};

struct co *co_head = NULL;
struct co *co_cur = NULL;
// reg_info *cur_reg = NULL;

struct co * random_select() {
  int i=1;
  struct co *ret=NULL;
  for(struct co * co_list=co_head;co_list;co_list=co_list->next){
    if(co_list->status == CO_DEAD)
      continue;
    if(rand()%i==0){
      ret=co_list;
    }
    i++;
  }
  return ret;
}

void _save(reg_info *cur_ctx)
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
    );
  #elif __i386__
    __asm__ __volatile__ (
    "       movl %esp, 0(%esi)          \n"    // save stack pointer
    "       movl %ebp, 4(%esi)          \n"    // save frame pointer
    "       movl (%esp), %eax           \n"
    "       movl %eax, 8(%esi)         \n"    // save pc pointer
    "       movl %ebx, 12(%esi)         \n"    // save rbx, r12 - r15
    );
  #endif
}
/*
 * %rdi: cur_ctx, %rsi: new_ctx
 * save current context to parameter_1: cur_ctx, switch context to parameter_2: new_ctx
 */
void _switch(reg_info *cur_ctx, reg_info *new_ctx)
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
    __asm__ __volatile__ (
    "       movl %esp, 0(%esi)          \n"    // save stack pointer
    "       movl %ebp, 4(%esi)          \n"    // save frame pointer
    "       movl (%esp), %eax           \n"
    "       movl %eax, 8(%esi)         \n"    // save pc pointer
    "       movl %ebx, 12(%esi)         \n"    // save rbx, r12 - r15
    "       movl 12(%edx), %ebx         \n"
    "       movl 4(%edx), %ebp          \n"    // restore frame pointer 
    "       movl 0(%edx), %esp          \n"    // restore stack pointer
    "       movl 8(%edx), %eax         \n"    // restore pc pointer
    "       movl %eax, (%esp)           \n"    // push pc pointer in stack
    "       ret                           "
    );
#endif
}

void _exec() {
  struct co_list *cur = NULL;
  co_cur->func(co_cur->arg);
  co_cur->status = CO_DEAD;
  debug(">>>=== CO_DEAD...... co_cur = %p\n",co_cur);
  //目前一个协程运行完成后，默认切回主程序
  _switch(co_cur->reg,co_head->reg);
}

struct co *co_start(const char *name, void (*func)(void *), void *arg) {
  //第一个node必然是当前main
  if(!co_head) {
    struct co *main_node = (struct co*)malloc(sizeof(struct co));
    main_node->stack = (uint8_t*)malloc(sizeof(uint8_t) * STACK_SIZE);
    main_node->reg = (reg_info*)malloc(sizeof(reg_info));

    main_node->name = "main";
    main_node->status = CO_RUNNING;
    main_node->last = NULL;
    main_node->next = NULL;
    co_head = main_node;
    co_cur = co_head;
  }
  struct co *ret = (struct co*)malloc(sizeof(struct co));
  ret->stack = (uint8_t*)malloc(sizeof(uint8_t) * STACK_SIZE);
  ret->reg = (reg_info*)malloc(sizeof(reg_info));

  ret->name = name;
  ret->func = func;
  ret->arg = arg;
  ret->status = CO_NEW;
  ret->next = NULL;
  ret->last = co_cur;
  // join list
  co_cur->next = ret;
  co_cur = ret;

  //设置协程的初始状态
#ifdef __x86_64__  
  ret->reg->rbp = ret->stack;
  ret->reg->rsp = ret->stack + STACK_SIZE - (sizeof(void *)*2);

  ret->reg->rip = (void *)_exec;
#elif __i386__
  ret->reg->ebp = ret->stack;
  ret->reg->esp = ret->stack + STACK_SIZE - (sizeof(void *)*2);

  ret->reg->eip = (void *)_exec;
#endif

  return ret;
}

void co_wait(struct co *co) {
  while(co->status != CO_DEAD) {
    //切换寄存器，保存运行状态到当前协程，将要运行的协程状态写入寄存器
    _switch(co_cur->reg,co->reg);
  }
  //remove list
  if(!co->last) {
    co->next->last = NULL;
  }
  else if(!co->next) {
    co->last->next = NULL;
  }
  else {
    co->last->next = co->next;
    co->next->last = co->last;
  } 
  //回收资源
  free(co->stack);
  free(co->reg);
  free(co);
  co = NULL;
}

void co_yield() {
  //随机选出下一个要运行的协程
  struct co *next = random_select();
  if(next) {
    //切换寄存器，保存运行状态到当前协程，将要运行的协程状态写入寄存器
    _switch(co_cur->reg,next->reg);
  }
  else {
    debug("no co can run.....\n");
  }

}
