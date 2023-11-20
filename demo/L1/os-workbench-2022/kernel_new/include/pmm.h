#include <common.h>

//所有的管理模块必须在堆上创建
//小于16M

enum {
    min = 0,
    nor,
    max
};
//空闲链表格式
struct mem_list{
    struct mem_list *next;
};
//内存块格式
typedef struct {
    struct mem_list *node;
    int size;
    bool used;
    void *ptr;
}mem_node;



/*创建三个链表，分别存放三类大小的内存
    0~128byte
    128byte~4K
    4K~16M
*/
struct mem_list *list_head[3] = {NULL,NULL,NULL};

// Spinlock
typedef int spinlock_t;

//三个链表对应的锁
spinlock_t *min_lock;
spinlock_t *nor_lock;
spinlock_t *max_lock;