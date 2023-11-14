
//所有的管理模块必须在堆上创建

//内存块格式
typedef struct {
    int size;
    bool used;
    // struct mem_head *next;
}mem_head;

typedef struct {
    mem_head head;
    void *ptr;
}mems;

//创建三个链表，分别存放三类大小的内存

//0~128byte
struct min_mem{
    mems mem_node;
    struct min_mem *next;
}min_mem_head;

//128byte~4K
struct normal_mem{
    mems mem_node;
    struct normal_mem *next;
}normal_mem_head;

//4K~16M
struct max_mem{
    mems mem_node;
    struct max_mem *next;
}max_mem_head;