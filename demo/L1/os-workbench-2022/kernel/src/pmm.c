#include <common.h>

spinlock_t *lock;
int len = 0;
int used_len = 0;

void spin_lock(spinlock_t *lk) {
  while (1) {
    intptr_t value = atomic_xchg(lk, 1);
    if (value == 0) {
      break;
    }
  }
}
void spin_unlock(spinlock_t *lk) {
  atomic_xchg(lk, 0);
}

void *kalloc(size_t size) {

  return NULL;
}

void kfree(void *ptr) {

}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  *lock = 0;    // unlock
  len = heap.end - heap.start;
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
