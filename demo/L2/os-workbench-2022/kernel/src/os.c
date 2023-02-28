#include <common.h>

#define MAXBLOCK 16384   //16*1024

extern Task_List *task_head;
extern Task_List *task_read;
extern spinlock_t *splk;
#define MAX_CPU 9

Task *currents[MAX_CPU];
#define current currents[cpu_current()]

irq_handle *ihandle = NULL;
irq_handle *ihandle_head = NULL;
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
      // 
      
        pmm->free(op.addr); 
        break;
      case OP_FREE:
        break;
      
    }
  }
}

static void os_run() {
  
  for (const char *s = "Hello World from CPU #*\n\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }
  //stress_test();
  
  while (1) {
    for (int volatile i = 0; i < 100000; i++) ;
    //printf("k %d\n",cpu_current());
  }
}

static Context *os_trap(Event ev, Context *context) {
  Context * cret;
  irq_handle *tmp = ihandle_head;
  //printf("7 %d\n",cpu_current());
  // kmt->spin_lock(splk);
  // printf("8 %d\n",cpu_current());
  if(!current) {
    //printf("77 %d \n",cpu_current());
    kmt->spin_lock(splk);
    current = task_read->cur;
    current->context = context;   //更新线程的运行状态
    
    task_read = task_read->next;
    if (!task_read) {
      task_read = task_head;    //循环秩序链表中的数据
    }
    //printf("88 %d %s\n",cpu_current(),current->name);
    kmt->spin_unlock(splk);
    //printf("33 %d \n",cpu_current());
    //printf("ba %d  %s\n",cpu_current(),current->name);
    return current->context;
  }
  else {
    current->context = context;   //更新线程的运行状态
  }
  kmt->spin_lock(splk);
  while(tmp) {
    if(tmp->event == ev.event) {
      cret = tmp->handler(ev,context);
      kmt->spin_unlock(splk);
      return cret;
    }
    tmp = tmp->next;
  }  
  //printf("55\n");
  //没有事件匹配，默认进行进程调度,调度当前线程的下一个
  do {
    // while( current == task_read->cur) {
    //   //printf("sd\n");
    //   task_read = task_read->next;
    //   if (!task_read) {
    //   task_read = task_head;    //循环秩序链表中的数据
    //   }
    // }
    current = task_read->cur;

    task_read = task_read->next;
    if (!task_read) {
      task_read = task_head;    //循环秩序链表中的数据
    }
    //printf("ab  %d \n",cpu_current());
    //printf(" current name = %s   %d \n",current->name,cpu_current());
    } while ((current->status != RUNNING));   //后期需要优化调度算法
  kmt->spin_unlock(splk);
  //printf("100 %d %s\n",cpu_current(),current->name);
  return current->context;
}
  
  

static void os_on_irq(int seq, int event, handler_t handler) {
  irq_handle *handle_cur = NULL;
  handle_cur = pmm->alloc(sizeof(irq_handle));
  handle_cur->seq = seq;
  handle_cur->event = event;
  handle_cur->handler = handler;
  handle_cur->next = NULL;

  if(!ihandle_head) 
    ihandle_head = ihandle;
  else
    ihandle->next = handle_cur;
  ihandle = handle_cur;

  return;
}
static Context *saved_context(Event ev, Context *context) {

  //printf("11 %d\n",cpu_current());
  //current->context = context;   //更新线程的运行状态
  do {
    // while( current == task_read->cur) {
    //   task_read = task_read->next;
    //   if (!task_read) {
    //   task_read = task_head;    //循环秩序链表中的数据
    //   }
    // }
    current = task_read->cur;

    task_read = task_read->next;
    if (!task_read) {
      task_read = task_head;    //循环秩序链表中的数据
    }

    //printf(" current name = %s   current->status  %d \n",current->name,current->status);
    } while ((current->status != RUNNING));   //后期需要优化调度算法
    //printf("22 %d  %s\n",cpu_current(),current->name);
    return current->context;
}

static void os_init() {
  pmm->init();
  kmt->init();

  os->on_irq(100, EVENT_YIELD, saved_context);
}
MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
  .trap = os_trap,
  .on_irq = os_on_irq,
};
