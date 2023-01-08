#include <common.h>

extern spinlock_t *lock;
static void os_init() {
  pmm->init();
}

static void os_run() {
  for (const char *s = "Hello World from CPU #*\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }
  spin_lock(lock);				//获取锁
  printf(">>>===111 into spin...\n");
  spin_unlock(lock);  			//释放锁
  while (1) ;
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
};
