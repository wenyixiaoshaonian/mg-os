#include <common.h>

typedef struct {
  size_t left;
  size_t right;
}used_heap;

typedef struct {
  used_heap data;
  used_heap next;
}used_list;

spinlock_t *slock;
size_t len = 0;
size_t used_len = 0;

typedef struct {  //20 bite
  size_t size;    //8 bite
  int status;     //4 bite
  void *adr;      //8 bite
} h_block;

h_block* find_block() {
  //todo
  return NULL;
}

h_block* create_block() {
  //todo
  return NULL;
}

h_block* break_block() {
  //todo
  return NULL;
}

void spin_init(spinlock_t *lk) {
  *lk = 0;
}
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
  //todo
  
  return NULL;
}

void kfree(void *ptr) {
  if(!ptr)
    return;
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);

  spin_init(slock);
  len = heap.end - heap.start;
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
