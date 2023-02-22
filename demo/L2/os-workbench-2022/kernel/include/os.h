#include <common.h>

enum os_status
{
  RUNNING = 1;
  WAITTING;
};


struct task {
  struct {
    int status;
    const char *name;
    union task *next;
    void      (*entry)(void *);
    Context    *context;
  };
  uint8_t stack[8192];
} Task;

typedef struct {
  Task *cur;
  Task *next;
}Task_List;


typedef struct {
  char *name;
  int lock;
  int locked;
  int lock_num;
  Task_List wait_list;
}spinlock;

// typedef struct  {
//   spinlock slock;
//   int locked;
//   Task_List wait_task;
// }metux;

struct semaphore {
  char *name;
//  int count;
  spinlock *slock;
};
