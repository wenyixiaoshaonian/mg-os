#include <common.h>

extern spinlock_t *lock;
static void os_init() {
  pmm->init();
}

static void os_run() {
  size_t a[3];
  for (const char *s = "Hello World from CPU #*\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }
  spin_lock(lock);				//获取锁
  printf(">>>===111 into spin...\n");
  // kalloc(1024);
  // kfree();
  spin_unlock(lock);  			//释放锁
  for(int i = 0;i<3;i++) {
    printf(">>>===333 a : %p...\n",&a[i]);
  }
  while (1) ;
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
};
