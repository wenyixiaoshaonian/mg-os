#include <common.h>



struct spinlock{
  const char *name;
  int lock;
  int locked;
  int lock_num;
  struct task_List *wait_list;
  struct task_List *waitlist_head;
};

struct semaphore {
  const char *name;
  struct spinlock *slock;
};