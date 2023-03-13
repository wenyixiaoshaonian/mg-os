#include <pmm.h>

#define SPIN_INIT(lk) \
  *lk = 0;

// #define SPIN_LOCK(lk) \
//   while (1) { \
//     intptr_t value = atomic_xchg(lk, 1); \
//     if (value == 0) return;}

void spin_lock(spinlock_p *lk) {
  while (1) {
    intptr_t value = atomic_xchg(lk, 1);
    if (value == 0) {
      return;
    }
  }
}
void spin_unlock(spinlock_p *lk) {
  atomic_xchg(lk, 0);
}

h_block* find_block(size_t size) {
  h_block* heap_block = NULL;
  if(!pmm_h.pre) {
    return NULL;
  }
  heap_block = pmm_h.head;
  
  while(heap_block) {
    if(heap_block->size == size && heap_block->status == FREE) {
      // printf("find the memory at %p...\n",heap_block);
      heap_block->status = USE;
      return heap_block;
    }
    heap_block = heap_block->next;
  }
  // printf("can not find the memory !!!\n");
  return NULL;
}

h_block* create_block(size_t size) {
  h_block* heap_block = NULL;
  if(pmm_h.used_len + HEADSIZE + size < pmm_h.len) {
    heap_block = heap.start + pmm_h.used_len;
    pmm_h.used_len += HEADSIZE + size;
    //printf(">>>=== %d  free size = %d \n",cpu_current(),len - pmm_h.used_len);
  }
  else {
    printf("there is not enough memory...\n");
    //iset(true);
    return NULL;
  }
  heap_block->size = size;
  heap_block->status = USE;
  heap_block->adr = (int)heap_block + HEADSIZE;
  heap_block->next = NULL;
  heap_block->use_flag = 0x55aa;   // this block is used
  if(pmm_h.pre)
    pmm_h.pre->next = heap_block;
  pmm_h.pre = heap_block;
//  printf("p88  %d\n",cpu_current());
  return heap_block;
}

bool free_block(h_block* block) {
  bool i = ienabled();
  if(block->status == FREE || block->use_flag != 0x55aa) {
    // printf(">>== %d  kfree failed.....%p  block->status = %d  block->use_flag %d\n",cpu_current(),block->adr,block->status,block->use_flag);
    return false;
  }
  block->status = FREE;
  return true;
}

h_block* break_block() {
  //todo
  return NULL;
}

void *kalloc(size_t size) {
  bool i = ienabled();
  iset(false);
  spin_lock(&pmm_h.slock);
  h_block* heap_block = find_block(size);
  if (heap_block) {
    spin_unlock(&pmm_h.slock);
    if (i)
      iset(true);
    return heap_block->adr;
  }
  else {
    heap_block = create_block(size);
    if (heap_block) {
      spin_unlock(&pmm_h.slock);
      if (i)
        iset(true);
      return heap_block->adr;
    }
  }
  printf("kalloc failed.....\n");
  spin_unlock(&pmm_h.slock);
  if (i)
    iset(true);
  return NULL;
}

void kfree(void *ptr) {
  
  if(!ptr) {
    return;
  }
  bool i = ienabled();
  iset(false);
  h_block* block = NULL;
  block = (h_block*)(ptr - HEADSIZE);   //根据adr找到block头部
  if(!free_block(block)) {
    if (i)
        iset(true);
    return;
  }
  if (i)
        iset(true);
// printf("kfree failed.....%p \n",ptr);
  return;
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  pmm_h.head = heap.start;
  pmm_h.len = heap.end - heap.start;
  SPIN_INIT(&pmm_h.slock);
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
