#include "pmm.h"

#define CPU_NUM 4
#define MAXBLOCK 16384   //16*1024

enum ops { OP_ALLOC = 0, OP_FREE };
struct malloc_op {
  enum ops type;
  struct { size_t sz; void *addr; };
};

extern struct mem_node *list_head[3];
struct malloc_op op[CPU_NUM];
void random_op() {
  // struct mem_list *list_tmp = NULL;
  struct mem_node *list_tmp = NULL;
  // struct mem_node *node_tmp = NULL;

  op[cpu_current()].type = rand() % 2;
  // printf(">>=== %d\n",op[cpu_current()].type);
  if(op[cpu_current()].type == OP_ALLOC) {
    op[cpu_current()].sz = rand() % MAXBLOCK;;
    op[cpu_current()].addr = NULL;
    return;
  }
  else {
    int num = rand() % 3;
    //如果当前链表没有数据，换下一个
    for(int i = 0;i < 3;i++ ) {
      if(list_head[num] == NULL) {
        num++;
        num = num % 3;
        continue;
      }
      list_tmp = list_head[num];
      while(list_tmp) {
        // node_tmp = list_tmp->mem;
        if(list_tmp->used){
          op[cpu_current()].addr = list_tmp->ptr;
          return;
        }
        list_tmp = list_tmp->next;
      }
    }   
    op[cpu_current()].addr = NULL;
    // printf("there is no memory need to free!\n");
    return;
  }
}

void stress_test() {
  while (1) {
    random_op();
    switch (op[cpu_current()].type) {
      case OP_ALLOC: 
        printf("%d  alloc %ld\n",cpu_current(),op[cpu_current()].sz);
        if(pmm->alloc(op[cpu_current()].sz) == NULL)
          // printf("%d alloc failed\n",cpu_current());
        // else
        //   printf("%d alloc success\n",cpu_current());
          
        break; //分配一块随即大小的内存
      case OP_FREE:  
        // printf("%d  free \n",cpu_current());
        if(op[cpu_current()].addr)
          pmm->free(op[cpu_current()].addr); 
        break;  //从链表中选择一块被使用的内存释放
    }
  }
}

static void os_init() {
  pmm->init();
}

static void os_run() {
  for (const char *s = "Hello World from CPU #*\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }
  stress_test();
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
};
