//#include <common.h>

#define STACK_SIZE 8192
#define MAX_CPU 8

extern Task_List *task_head;
extern Task_List *task_pre;
extern Task_List *task_read;

extern spinlock_t splk;
extern sem_t semlk;
