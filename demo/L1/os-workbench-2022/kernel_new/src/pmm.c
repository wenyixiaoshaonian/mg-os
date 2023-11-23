
#include "pmm.h"

void lock(spinlock_t *locked)   { while (atomic_xchg(locked, 1)); }
void unlock(spinlock_t *locked) { atomic_xchg(locked, 0); }

//三个链表对应的锁
spinlock_t *min_lock;
spinlock_t *nor_lock;
spinlock_t *max_lock;

spinlock_t *alloc_lock;
/*创建三个链表，分别存放三类大小的内存
    0~128byte
    128byte~4K
    4K~16M
*/
struct mem_node *list_head[3] = {NULL,NULL,NULL};
struct mem_node *list_cur[3] = {NULL,NULL,NULL};
// int mem_used = 0;
void * ptr;
static void *kalloc(size_t size) {
  // struct mem_list *list_tmp = NULL;
  struct mem_node *list_tmp = NULL;
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
  printf(">>==11\n");
  lock(tmp_lock);
  if(list_head[number]) {
    list_tmp = list_head[number];
    printf(">>==aa %d\n",number);
    while(list_tmp) {
      if(list_tmp > 0x07f00000)
        printf("list_tmp = %p size = %d \n",list_tmp,list_tmp->size);
      // node_tmp = list_tmp->mem;
      if(list_tmp->size == size && !list_tmp->used) {
        list_tmp->used = 1;
        printf(">>==22\n");
        unlock(tmp_lock);
        return list_tmp->ptr;
      }
      list_tmp = list_tmp->next;
    }
  }
  unlock(tmp_lock);
  printf(">>==33\n");
  lock(alloc_lock);
  alloc_size = sizeof(struct mem_node) + size;
  if(ptr + alloc_size > heap.end) {
    printf("Insufficient memory space!\n");
    unlock(tmp_lock);
    return NULL;
  }
  // mem_used +=alloc_size;
  printf(" start = 0x%p  size = 0x%x\n",ptr,alloc_size);
  //申请空间分布  head -- body
  node_tmp = (struct mem_node *)ptr;
  node_tmp->ptr = (void*)((char *)ptr + sizeof(struct mem_node));
  // node_tmp->node = (struct mem_list *)((char *)ptr + sizeof(struct mem_node) + size);
  ptr += alloc_size;
  unlock(alloc_lock);

  lock(tmp_lock);
  // node_tmp->node->mem = (struct mem_list *)node_tmp;
  // node_tmp->node->next = NULL;
  // list_tmp = node_tmp->node;
  printf(">>==00 node_tmp = %p  list_head[%d] = %p\n",node_tmp,number,list_head[number]);
  if(!list_head[number]) {
    list_head[number] = node_tmp;
    list_cur[number] = node_tmp;
    // printf(">>==11 list_head[%d] = %p\n",number,list_head[number]);
  }
  else {
    list_cur[number]->next = node_tmp;
    list_cur[number] = node_tmp;
  }
  // printf(">>==33 list_head[%d] = %p\n",number,list_head[number]);
  node_tmp->size = size;
  node_tmp->used = 1;
  unlock(tmp_lock);
  return node_tmp->ptr;
  
}

static void kfree(void *ptr) {
  struct mem_node *node_tmp = NULL;
  spinlock_t *tmp_lock;
  printf(">>==55 ptr = %p  size = 0x%x \n",ptr,sizeof(struct mem_node));
  node_tmp = (struct mem_node *)((char *)(ptr) - sizeof(struct mem_node));
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
  printf("dd  %p size = 0x%x\n",node_tmp,size);
  lock(tmp_lock);
  node_tmp->used = 0;
  unlock(tmp_lock);
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  ptr = heap.start;
  unlock(min_lock);
  unlock(nor_lock);
  unlock(max_lock);
  unlock(alloc_lock);

}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
