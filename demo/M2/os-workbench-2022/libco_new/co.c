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
    void *r12d;
    void *r13d;
    void *r14d;
    void *r15d;
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
reg_info *cur_reg = NULL;

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

struct co *co_start(const char *name, void (*func)(void *), void *arg) {
  //第一个node必然是当前main
  if(!co_head) {
    struct co *main_node = (struct co*)malloc(sizeof(struct co));
    main_node->stack = (uint8_t*)malloc(sizeof(uint8_t) * STACK_SIZE);
    main_node->reg = (reg_info*)malloc(sizeof(reg_info));

    main_node->name = "main";
    // main_node->func = func;
    // main_node->arg = arg;
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

  return ret;
}

void co_wait(struct co *co) {
    //save(cur_reg);
  while(co->status != CO_DEAD) {
    //切换寄存器，保存运行状态到当前协程，将要运行的协程状态写入寄存器
    // yield(cur->reg,next->reg);
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

  //保存当前协程运行状态，将当前寄存器拷贝至内存
  // save(cur_reg);

  //切换寄存器，保存运行状态到当前协程，将要运行的协程状态写入寄存器
  // yield(cur->reg,next->reg);
}
