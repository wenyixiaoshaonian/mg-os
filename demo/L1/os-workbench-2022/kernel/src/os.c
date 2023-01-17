#include <common.h>

// extern spinlock_t *slock;
static void os_init() {
  pmm->init();
}

static void os_run() {
  void *test = NULL;
  void *test2 = NULL;
  for (const char *s = "Hello World from CPU #*\n\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }
  // spin_lock(slock);				//获取锁
  test = kalloc(1024);
  printf(">>>===111 adr = %p...\n\n",test);

  test2 = kalloc(1024);
  printf(">>>===111 adr = %p...\n\n",test2);

  kfree(test);
  

  test = kalloc(1024);
  printf(">>>===111 adr = %p...\n\n",test);

  kfree(test);
  kfree(test2);
  // spin_unlock(slock);  			//释放锁
  while (1) ;
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
};
