#include <common.h>

spinlock_t *slock;
size_t len = 0;
size_t used_len = 0;

typedef struct x_block h_block;

struct x_block {  // 32 bite
  size_t size;    // 8 bite
  int status;     // 4 bite
  void *adr;      // 8 bite
  h_block *next;  // 8 bit
  int reserve;    // 4 bit
};

h_block *head = NULL;
h_block *pre = NULL;

#define HEADSIZE 32
#define USE 1
#define FREE 0

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

h_block* find_block(size_t size) {
  h_block* heap_block = NULL;
  if(!pre) {
    printf("can not find the memory !!!\n");
    return NULL;
  }
    
  heap_block = head;
  spin_lock(slock);
  while(heap_block) {
    if(heap_block->size == size && heap_block->status == FREE) {
      printf("find the memory at %p...\n",heap_block);
      heap_block->status = USE;
      return heap_block;
    }
    // printf("memory at %p...status = %d\n",heap_block,heap_block->status);
    heap_block = heap_block->next; 
  }
  spin_unlock(slock);
  printf("can not find the memory !!!\n");
  return NULL;
}

h_block* create_block(size_t size) {
  h_block* heap_block = NULL;

  spin_lock(slock);
  if(used_len + HEADSIZE + size < len) {
    heap_block = heap.start + used_len;
    used_len += HEADSIZE + size;
  }
  else {
    printf("there is not enough memory...\n");
    return NULL;
  }
  heap_block->size = size;
  heap_block->status = USE;
  heap_block->adr = (int)heap_block + HEADSIZE;
  heap_block->next = NULL;
  if(pre)
    pre->next = heap_block;
  pre = heap_block;
  spin_unlock(slock);
  return heap_block;
}

void free_block(h_block* block) {
  spin_lock(slock);
  block->status = FREE;
  // printf("free_block at %p...status = %d\n",block,block->status);
  spin_unlock(slock);
  return;
}

h_block* break_block() {
  //todo
  return NULL;
}

void *kalloc(size_t size) {
  h_block* heap_block = find_block(size);
  if (heap_block)
    return heap_block->adr;
  else {
    heap_block = create_block(size);
    if (heap_block)
      return heap_block->adr;
  }
  return NULL;
}

void kfree(void *ptr) {
  h_block* block = NULL;
  block = (h_block*)(ptr - 0x20);   //根据adr找到block头部
  if(!block)
    return;
  else {
    free_block(block);
  }
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
