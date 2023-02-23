#include <common.h>

spinlock_p *slock;
size_t len = 0;
size_t used_len = 0;

typedef struct x_block h_block;

struct x_block {  // 40 bite
  size_t size;    // 8 bite
  int status;     // 4 bite
  void *adr;      // 8 bite
  h_block *next;  // 8 bit
  int use_flag;    // 4 bit
};

h_block *head = NULL;
h_block *pre = NULL;
#define HEADSIZE sizeof(h_block)
#define USE 1
#define FREE 0

void spin_init(spinlock_p *lk) {
  *lk = 0;
}
void spin_lock(spinlock_p *lk) {
  while (1) {
    intptr_t value = atomic_xchg(lk, 1);
    if (value == 0) {
      break;
    }
  }
}
void spin_unlock(spinlock_p *lk) {
  atomic_xchg(lk, 0);
}

h_block* find_block(size_t size) {
  h_block* heap_block = NULL;
  bool i = ienabled();
  if(!pre) {
    return NULL;
  }
  heap_block = head;
  iset(false);
  spin_lock(slock);
  while(heap_block) {
    if(heap_block->size == size && heap_block->status == FREE) {
      // printf("find the memory at %p...\n",heap_block);
      heap_block->status = USE;
      spin_unlock(slock);
      iset(true);
      return heap_block;
    }
    // printf("memory at %p...status = %d\n",heap_block,heap_block->status);
    heap_block = heap_block->next;
  }
  spin_unlock(slock);
  if (i)
    iset(true);
  // printf("can not find the memory !!!\n");
  return NULL;
}

h_block* create_block(size_t size) {
  h_block* heap_block = NULL;
  bool i = ienabled();
  iset(false);
  spin_lock(slock);
  if(used_len + HEADSIZE + size < len) {
    heap_block = heap.start + used_len;
    used_len += HEADSIZE + size;
    //printf(">>>=== %d  free size = %d \n",cpu_current(),len - used_len);
  }
  else {
    printf("there is not enough memory...\n");
    spin_unlock(slock);
    iset(true);
    return NULL;
  }
  heap_block->size = size;
  heap_block->status = USE;
  heap_block->adr = (int)heap_block + HEADSIZE;
  heap_block->next = NULL;
  heap_block->use_flag = 0x55aa;   // this block is used
  if(pre)
    pre->next = heap_block;
  pre = heap_block;
  // printf(">>== %d   %p  block->use_flag %d\n",cpu_current(),heap_block->adr,heap_block->use_flag);
  spin_unlock(slock);
  if (i)
    iset(true);
  return heap_block;
}

bool free_block(h_block* block) {
  bool i = ienabled();
  iset(false);
  spin_lock(slock);
  if(block->status == FREE || block->use_flag != 0x55aa) {
    // printf(">>== %d  kfree failed.....%p  block->status = %d  block->use_flag %d\n",cpu_current(),block->adr,block->status,block->use_flag);
    spin_unlock(slock);
    iset(true);
    return false;
  }
  block->status = FREE;
  spin_unlock(slock);
  if (i)
    iset(true);
  return true;
}

h_block* break_block() {
  //todo
  return NULL;
}

void *kalloc(size_t size) {
  h_block* heap_block = find_block(size);
  if (heap_block) {
    return heap_block->adr;
  }
  else {
    heap_block = create_block(size);
    if (heap_block) {
      return heap_block->adr;
    }
  }
  printf("kalloc failed.....\n");
  return NULL;
}

void kfree(void *ptr) {
  
  if(!ptr) {
    return;
  }
  h_block* block = NULL;
  block = (h_block*)(ptr - HEADSIZE);   //根据adr找到block头部
  if(!free_block(block)) {
    return;
  }
    // printf("kfree failed.....%p \n",ptr);
  
  return;
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  spin_init(slock);
  head = heap.start;
  len = heap.end - heap.start;
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
