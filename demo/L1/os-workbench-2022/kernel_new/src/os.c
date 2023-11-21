#include "pmm.h"

#define CPU_NUM 4
#define MAXBLOCK 16384   //16*1024

enum ops { OP_ALLOC = 1, OP_FREE };
struct malloc_op {
  enum ops type;
  struct { size_t sz; void *addr; };
};

extern struct mem_list *list_head[3];
struct malloc_op op[CPU_NUM];
void random_op() {
  struct mem_list *list_tmp = NULL;
  struct mem_node *node_tmp = NULL;

  op[cpu_current()].type = rand() % 2;
  if(op[cpu_current()].type == OP_ALLOC) {
    op[cpu_current()].sz = rand() % MAXBLOCK;;
    op[cpu_current()].addr = NULL;
  }
  else {
    int num = rand() % 3;
    //如果当前链表没有数据，换下一个
    for(int i = 0;i < 3;i++ ) {
      if(list_head[num] == NULL) {
        num++;
        num = num % 3;
        break;
      }
      list_tmp = list_head[num];
      while(list_tmp) {
        node_tmp = list_tmp->mem;
        if(node_tmp->used){
          op[cpu_current()].addr = node_tmp->ptr;
          node_tmp->used = 0;
          return;
        }
        list_tmp = list_tmp->next;
      }
    }   
    printf("there is no memory need to free!\n");
    return;
  }
}

void alloc_check(size_t org,size_t dst) {
  if(org != dst)
    printf("malloc size is wrong\n");
}

void stress_test() {
  while (1) {
    random_op();
    switch (op[cpu_current()].type) {
      case OP_ALLOC: pmm->alloc(op[cpu_current()].sz); break;
      case OP_FREE:  pmm->free(op[cpu_current()].addr); break;
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
  while (1) {
	//todo test
	
	};
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
};
