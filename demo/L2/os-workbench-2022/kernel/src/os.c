#include <common.h>

#define MAXBLOCK 16384   //16*1024

extern Task_List *task_head;
extern Task_List *task_read;
extern spinlock_t splk;
#define MAX_CPU 9

Task *currents[MAX_CPU];
#define current currents[cpu_current()]

Task_List *task_reads[MAX_CPU];
#define reads task_reads[cpu_current()]

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
  for (int volatile i = 0; i < 10000; i++) ;
  iset(true);
  while (1) {
    for (int volatile i = 0; i < 10000000; i++) ;
    printf("k %d\n",cpu_current());
  }
}

static Context *os_trap(Event ev, Context *context) {
  Context * cret;
  irq_handle *tmp = ihandle_head;
  // printf("ev %d  %d\n",ev,ihandle_head->event);
  // kmt->spin_lock(&splk);
  // printf("8 %d\n",cpu_current());
  if(!current) {
    kmt->spin_lock(&splk);
    do{
    //printf("77 %d \n",cpu_current());
    current = task_read->cur;
    // if(current->name == "main")
    //   current->context = context;   //主线程 更新线程的运行状态
    
    task_read = task_read->next;
    if (!task_read) {
      task_read = task_head;    //循环秩序链表中的数据
    }
    // printf("task_read - task_head  %d  %d\n",((task_read - task_head)/520) % cpu_count(), cpu_current());
    //} while (((task_read - task_head)/520) % cpu_count() != cpu_current());
    } while ((current->status != RUNNING) || ((current->call_status != CALLABLE)));
    //printf("88 %d  %s\n",cpu_current(),current->name);
    current->call_status = UNCALLABLE;
    kmt->spin_unlock(&splk);
    return current->context;
  }
  kmt->spin_lock(&splk);
  if(current->call_status == UNCALLABLE) {
    current->context = context;   //更新线程的运行状态
    current->call_status = CALLABLE;
  }
  else
    printf("call_status error %d\n", cpu_current());

  while(tmp) {
    if(tmp->event == ev.event) {
      kmt->spin_unlock(&splk);
      cret = tmp->handler(ev,context);
      return cret;
    }
    tmp = tmp->next;
  }
  //没有事件匹配，默认进行进程调度,调度当前线程的下一个
  do {
    current = task_read->cur;

    task_read = task_read->next;
    if (!task_read) {
      task_read = task_head;
    }
    //printf("task_read - task_head  %d  %d\n",((task_read - task_head)/520) % cpu_count(), cpu_current());
  //} while (((task_read - task_head)/520) % cpu_count() != cpu_current());
  } while ((current->status != RUNNING) || ((current->call_status != CALLABLE)));
  //printf("1000 %d %s\n",cpu_current(),current->name);
  current->call_status = UNCALLABLE;
  kmt->spin_unlock(&splk);
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
    ihandle_head = handle_cur;
  else
    ihandle->next = handle_cur;
  ihandle = handle_cur;

  return;
}
static Context *saved_context(Event ev, Context *context) {
  // printf("saved_context %d\n",cpu_current());
  //current->context = context;   //更新线程的运行状态
  kmt->spin_lock(&splk);
  do {
    current = task_read->cur;

    task_read = task_read->next;
    if (!task_read) {
      task_read = task_head;    //循环秩序链表中的数据
    }
    //printf(" current name = %s   current->status  %d \n",current->name,current->status);
    } while ((current->status != RUNNING) || ((current->call_status != CALLABLE)));
    printf("111 yied %d  %s\n",cpu_current(),current->name);
    current->call_status = UNCALLABLE;
    kmt->spin_unlock(&splk);
    return current->context;
}

static void os_init() {
  pmm->init();
  printf("pmm init finished\n");
  kmt->init();
  printf("kmt init finished\n");
  // dev->init();
  // printf("dev init finished\n");
  os->on_irq(100, EVENT_YIELD, saved_context);
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
  .trap = os_trap,
  .on_irq = os_on_irq,
};
