#include <common.h>

extern spinlock_t *slock;
static void os_init() {
  pmm->init();
}

static void os_run() {
  void *test = NULL;
  for (const char *s = "Hello World from CPU #*\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }
  spin_lock(slock);				//获取锁
  test = kalloc(1024);
  printf(">>>===111 test = %p...\n",test);
  kfree(test);
  spin_unlock(slock);  			//释放锁
  while (1) ;
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
};
