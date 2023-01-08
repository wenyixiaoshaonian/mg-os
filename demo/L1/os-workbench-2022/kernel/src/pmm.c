#include <common.h>

spinlock_t *lock;
size_t len = 0;
size_t used_len = 0;
size_t start,end;

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
  if(len <= 0)
    return NULL;
  
  return NULL;
}

void kfree(void *ptr) {
  if(!ptr)
    return;
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  *lock = 0;    // unlock
  len = heap.end - heap.start;
  used_len = 0;
  start = (int)heap.start;
  end = (int)heap.end;
  printf("start %d end %d \n", pmsize >> 20, start, end);
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
