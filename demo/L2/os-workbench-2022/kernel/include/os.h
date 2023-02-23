#include <common.h>

struct task_List{
  struct task *cur;
  struct task *next;
};

struct spinlock{
  const char *name;
  int lock;
  int locked;
  int lock_num;
  struct task_List *wait_list;
};

struct semaphore {
  const char *name;
  struct spinlock *slock;
};