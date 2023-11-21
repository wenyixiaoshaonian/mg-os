
#include "pmm.h"

void lock(spinlock_t *locked)   { while (atomic_xchg(locked, 1)); }
void unlock(spinlock_t *locked) { atomic_xchg(locked, 0); }

//三个链表对应的锁
spinlock_t *min_lock;
spinlock_t *nor_lock;
spinlock_t *max_lock;

/*创建三个链表，分别存放三类大小的内存
    0~128byte
    128byte~4K
    4K~16M
*/
struct mem_list *list_head[3] = {NULL,NULL,NULL};
struct mem_list *list_cur[3] = {NULL,NULL,NULL};

static void *kalloc(size_t size) {
  struct mem_list *list_tmp = NULL;
  struct mem_node *node_tmp = NULL;
  int number;
  int alloc_size = 0;
  spinlock_t *tmp_lock;
  if(size <= 128) {
    number = min;
    tmp_lock = min_lock;
  }
  else if(size <= 4*1024) {
    number = nor;
    tmp_lock = nor_lock;
  }
  else if(size <= 16*1024*1024) {
    number = max;
    tmp_lock = max_lock;
  }
  else {
    printf("size is too big!!\n");
    return NULL;
  }

  lock(tmp_lock);
  if(list_head[number]) {
    list_tmp = list_head[number];
    while(list_tmp) {
      node_tmp = list_tmp->mem;
      if(node_tmp->size == size && !node_tmp->used) {
        node_tmp->used = 1;
        return node_tmp->ptr;
      }
      list_tmp = list_tmp->next;
    }
  }
  alloc_size = sizeof(struct mem_node) + sizeof(struct mem_list) + size;
  if(heap.start + alloc_size > heap.end) {
    printf("Insufficient memory space!\n");
    return NULL;
  }
  node_tmp = heap.start;
  heap.start += alloc_size;

  list_tmp = node_tmp->node;
  list_tmp->next = NULL;
  if(!list_head[number]) {
    list_head[number] = list_tmp;
    list_cur[number] = list_tmp;
  }
  else {
    list_cur[number]->next = list_tmp;
    list_cur[number] = list_tmp;
  }
  node_tmp->size = size;
  node_tmp->used = 1;
  node_tmp->ptr = (void*)((char *)node_tmp + sizeof(struct mem_node) + sizeof(struct mem_list));
  unlock(tmp_lock);
  return node_tmp->ptr;
  
  return NULL;
}

static void kfree(void *ptr) {
  struct mem_node *node_tmp = NULL;
  spinlock_t *tmp_lock;
  
  node_tmp = ((char *)ptr - sizeof(struct mem_node) + sizeof(struct mem_list));
  int size = node_tmp->size;
  if(size <= 128) {
    tmp_lock = min_lock;
  }
  else if(size <= 4*1024) {
    tmp_lock = nor_lock;
  }
  else if(size <= 16*1024*1024) {
    tmp_lock = max_lock;
  }

  lock(tmp_lock);
  node_tmp->used = 0;
  unlock(tmp_lock);

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
