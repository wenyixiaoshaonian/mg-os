#include <common.h>

//所有的管理模块必须在堆上创建
//小于16M

enum {
    min = 0,
    nor,
    max
};

struct mem_node;
//空闲链表格式
struct mem_list{
    struct mem_node *mem;
    struct mem_list *next;
};
//内存块格式
struct mem_node{
    struct mem_list *node;
    int size;
    bool used;
    void *ptr;
};


// Spinlock
typedef int spinlock_t;

