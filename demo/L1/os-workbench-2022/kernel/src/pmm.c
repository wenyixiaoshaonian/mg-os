#include <common.h>

spinlock_t *lock = 0;

void spin_lock(spinlock_t *lk) {
  while (1) {
    printf("111 lk : %d\n", lk);
    intptr_t value = atomic_xchg(lk, 1);
    printf("222 lk : %d\n", lk);
    if (value == 0) {
      printf("break.....\n");
      break;
    }
  }
}
void spin_unlock(spinlock_t *lk) {
  atomic_xchg(lk, 0);
}
static void *kalloc(size_t size) {
  return NULL;
}

static void kfree(void *ptr) {
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)   lock : %d\n", pmsize >> 20, heap.start, heap.end,lock);
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
