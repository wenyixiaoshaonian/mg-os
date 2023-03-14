#include <kernel.h>
#include <klib.h>
#include <klib-macros.h>



typedef struct irq_h irq_handle;
typedef struct task Task;
typedef struct task_List Task_List;

struct spinlock{
  const char *name;
  int lock;
  int locked;
  int lock_num;
  int irq_flag;     //中断中使用的锁不能进行任务调度，用户进程的可以
  struct task_List *wait_list;
  struct task_List *waitlist_read;
  struct task_List *waitlist_head;
};

struct semaphore {
  const char *name;
  struct spinlock slock;
};


enum os_status
{
  RUNNING = 1,
  WAITTING,
};
enum call_status
{
  CALLABLE = 1,
  UNCALLABLE,
};

struct task_List{
    struct task *cur;
    struct task_List *next;
};

struct irq_h{
  int seq;
  int event;
  handler_t handler;
  struct irq_h *next;
};

struct task {
  struct {
    int status;
    int call_status;
    const char *name;
    // struct task *next;
    void      (*entry)(void *);
    Context    *context;
  };
  uint8_t stack[8192];
};

