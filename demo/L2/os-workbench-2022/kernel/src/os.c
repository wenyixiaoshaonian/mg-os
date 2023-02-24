#include <common.h>

#define MAXBLOCK 16384   //16*1024


extern spinlock_p *slock;
extern Task_List *task_head;
extern Task_List *task_read;
extern spinlock_t *splk;
extern sem_t *semlk;
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

static void producer(void *arg) {
  while(1) {
    kmt->sem_signal(semlk);
    printf("(");
    for (int volatile i = 0; i < 100000; i++) ;
  }
  
  }

static void consumer(void *arg) {
  while(1) {
    kmt->sem_wait(semlk);
    printf(")");
    for (int volatile i = 0; i < 100000; i++) ;
  }
}



static void os_run() {
  
  for (const char *s = "Hello World from CPU #*\n\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }
//  stress_test();

  kmt->create(pmm->alloc(sizeof(task_t)),
              "test-thread-1", producer, NULL);
  kmt->create(pmm->alloc(sizeof(task_t)),
              "test-thread-2", consumer, NULL);
  kmt->create(pmm->alloc(sizeof(task_t)),
              "test-thread-3", producer, NULL);
  kmt->create(pmm->alloc(sizeof(task_t)),
              "test-thread-4", consumer, NULL);

  // kmt->create(pmm->alloc(sizeof(task_t)),
  //             "test-thread-5", consumer, NULL);
  // kmt->create(pmm->alloc(sizeof(task_t)),
  //             "test-thread-6", consumer, NULL);
  // kmt->create(pmm->alloc(sizeof(task_t)),
  //             "test-thread-7", consumer, NULL);
  // kmt->create(pmm->alloc(sizeof(task_t)),
  //             "test-thread-8", consumer, NULL);
  //printf(">>>====kmt create finished\n");
  iset(true);
  while (1) {
    ;
  }
}

static Context *os_trap(Event ev, Context *context) {
  Context * cret;
  irq_handle *tmp = ihandle_head;
  //printf("os_trap start....  event = %d\n",ev);
  kmt->spin_lock(splk);
  //printf(" current  = %p   %d\n",current,cpu_current());
  if(!current) {
    current = task_read->cur;     //从主线程进入，无需更新主进程的状态
    task_read = task_read->next;
    if (!task_read) {
      task_read = task_head;    //循环秩序链表中的数据
    }
    kmt->spin_unlock(splk);
    return current->context;
  }
  else {
    current->context = context;   //更新线程的运行状态
  }
  // task_read = task_read->next;
  // if (!task_read) {
  //   task_read = task_head;    //循环秩序链表中的数据
  // }
  //printf("current name = %p\n",current);
  

  while(tmp) {
    if(tmp->event == ev.event) {
      kmt->spin_unlock(splk);
      cret = tmp->handler(ev,context);
      return cret;
    }
    tmp = tmp->next;
  }  
  //kmt->spin_lock(splk);
  //没有事件匹配，默认进行进程调度,调度当前线程的下一个
  do {
    while( current == task_read->cur) {
      task_read = task_read->next;
      if (!task_read) {
      task_read = task_head;    //循环秩序链表中的数据
      }
    }
    current = task_read->cur;

    task_read = task_read->next;
    if (!task_read) {
      task_read = task_head;    //循环秩序链表中的数据
    }

    //printf(" current name = %s   %d \n",current->name,cpu_current());
    } while ((current->status != RUNNING));   //后期需要优化调度算法
  kmt->spin_unlock(splk);
  //printf("current name = %s \n",current->name);
  return current->context;
}
  
  

static void os_on_irq(int seq, int event, handler_t handler) {
  ihandle = pmm->alloc(sizeof(irq_handle));
  ihandle->seq = seq;
  ihandle->event = event;
  ihandle->handler = handler;
  ihandle->next = NULL;

  if(!ihandle_head) 
    ihandle_head = ihandle;

  ihandle = ihandle->next;

  return;
}
static Context *saved_context(Event ev, Context *context) {

  //printf(">>>====  saved_context \n");
  kmt->spin_lock(splk);
  current->context = context;   //更新线程的运行状态
  do {
    while( current == task_read->cur) {
      task_read = task_read->next;
      if (!task_read) {
      task_read = task_head;    //循环秩序链表中的数据
      }
    }
    current = task_read->cur;

    task_read = task_read->next;
    if (!task_read) {
      task_read = task_head;    //循环秩序链表中的数据
    }

    //printf(" current name = %s   current->status  %d \n",current->name,current->status);
    } while ((current->status != RUNNING));   //后期需要优化调度算法
    kmt->spin_unlock(splk);
    //printf(">>>====  saved_context finished\n");
    return current->context;
}

// extern spinlock_p *slock;
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
