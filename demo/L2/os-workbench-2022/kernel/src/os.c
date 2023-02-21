#include <common.h>

#define MAXBLOCK 16384   //16*1024
#define MAX_CPU 8

extern spinlock_p *slock;

Task *currents[MAX_CPU];
#define current currents[cpu_current()]

enum ops { OP_ALLOC = 0, OP_FREE };
struct malloc_op {
  enum ops type;
  size_t sz; 
  void *addr; 
   ;
};

void random_op(struct malloc_op* op) {
  // op->type = rand() % 2;
  op->sz = rand() % MAXBLOCK;
  op->addr = NULL;
  return ;
}

int alloc_check(void *adr,size_t sz) {
  if(!adr)
    return false;
  size_t size = adr - 40;
  return (adr > heap.start && adr < heap.end && size == sz) ? true:false;
}

static void stress_test() {
  struct malloc_op op;
  op.type = OP_ALLOC;
  
  while (1) {
    random_op(&op);
    switch (op.type) {
      case OP_ALLOC: 
        op.addr = pmm->alloc(op.sz);
        if(!op.addr && !alloc_check(op.addr, op.sz)) {
          printf("malloc adr is illegality!!!!!!\n");
          return;
        }
      // printf("%d malloc adr successful  adr = %p  size = %d\n",cpu_current(),op.addr,op.sz);
      // break;
      // case OP_FREE:
        kfree(op.addr); 

      break;
    }
  }
}

// extern spinlock_p *slock;
static void os_init() {
  pmm->init();
  kmt->init();
}

static void os_run() {
  void *test = NULL;
  void *test2 = NULL;
  for (const char *s = "Hello World from CPU #*\n\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }
  stress_test();
  while (1) {
    ;
  }
}

static Context os_trap(Event ev, Context *context) {
  extern Task tasks[];
  if (!current)
    current = &tasks[0];
  else
    current->context = ctx;   //更新线程的运行状态
  do {
    current = current->next;
  } while ((current - tasks) % cpu_count() != cpu_current());   //后期需要优化调度算法
  return current->context;
}

static void os_on_irq(int seq, int event, handler_t handler) {

  return;
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
  .trap = os_trap,
  .on_irq = os_on_irq,
};
