#include <common.h>

struct task {
  struct {
    const char *name;
    union task *next;
    void      (*entry)(void *);
    Context    *context;
  };
  uint8_t stack[8192];
} Task;

struct spinlock {
  char *name;
  int lock;
  // TODO
};

struct semaphore {
  char *name;
  int count;
  spinlock *slock;
  // TODO
};
