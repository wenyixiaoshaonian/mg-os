
#include "pmm.h"

void lock(spinlock_t *locked)   { while (atomic_xchg(locked, 1)); }
void unlock(spinlock_t *locked) { atomic_xchg(locked, 0); }

/*创建三个链表，分别存放三类大小的内存
    0~128byte
    128byte~4K
    4K~16M
*/
struct mem_list *list_head[3] = {NULL,NULL,NULL};

static void *kalloc(size_t size) {
  return NULL;
}

static void kfree(void *ptr) {
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);

  unlock(min_lock);
  unlock(nor_lock);
  unlock(max_lock);

}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
