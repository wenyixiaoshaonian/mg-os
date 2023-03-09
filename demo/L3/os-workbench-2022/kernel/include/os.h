#include <common.h>

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