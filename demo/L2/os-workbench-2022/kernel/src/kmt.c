#include <os.h>

#define STACK_SIZE 8192

#define MAX_CPU 8

extern Task *currents[MAX_CPU];
#define current currents[cpu_current()]

Task_List *task_head = NULL;
Task_List *task_pre = NULL;
Task_List *task_read = NULL;
spinlock_t *splk = NULL;
sem_t *semlk = NULL;

void enqueue(Task_List *list,Task *cur) {
    Task_List *task_cur = (task_t *)pmm->alloc(sizeof(Task_List));
    task_cur->cur = task_cur;
    task_cur->next = NULL;
    if(!list->list_head) {
        list->list_head = task_cur;
    }
    list = task_cur;
    list = list->next;
}

Task *dequeue(Task_List *list) {
    Task *ret = NULL;
    ret = list->list_head->cur;
    list->list_head = list->list_head->next;
    return ret;
}

/*---------------------------------------spin-------------------------------------------------------*/
static void spin_lock(int *lock) {
    iset(false);
    while (1) {
    intptr_t value = atomic_xchg(lock, 1);
        if (value == 0) {
            iset(true);
            return;
        } 
    }
}

static void spin_unlock(int *lock) {
    iset(false);
    atomic_xchg(lock, 0);
    iset(true);
}
/*---------------------------------------metux-------------------------------------------------------*/
static void kmt_spin_init(spinlock_t *lk, const char *name) {
    lk = (spinlock_t *)pmm->alloc(sizeof(spinlock_t));
    lk->name = name;
    lk->lock = 0;       //解锁状态
    lk->locked = 0;     //解锁状态
    lk->lock_num = 1;   //当作互斥锁使用
    lk->wait_list = (Task_List *)NULL;
}
static void kmt_spin_lock(spinlock_t *lk) {
    int acq = 0;
    spin_lock(&lk->lock);
    if(lk->locked >= lk->lock_num) {
        current->status = WAITTING;
        enqueue(lk->wait_list, current);        //添加到等待队列
    }
    else {
        lk->locked ++;
        acq = 1;
    }
    spin_unlock(&lk->lock);
    if(acq)
        yield(); // 阻塞时切换
}

static void kmt_spin_unlock(spinlock_t *lk) {
    spin_lock(&lk->lock);
    if(lk->wait_list->list_head) {
        Task *task = dequeue(lk->wait_list);
        task->status = RUNNING;
    }
    else if(lk->locked -1  >= 0) 
        lk->locked --;
    else
        lk->locked = 0;
    spin_unlock(&lk->lock);
}

/*---------------------------------------sem-------------------------------------------------------*/
static void kmt_sem_init(sem_t *sem, const char *name, int value) {
    sem = (sem_t *)pmm->alloc(sizeof(sem_t));
    sem->name = name;
    //sem_t->count = value;
    sem->slock->lock = 0;             //解锁状态
    sem->slock->locked = 0;           //解锁状态
    sem->slock->lock_num = value;     //当作信号量使用
    sem->slock->wait_list = NULL;
}

static void kmt_sem_wait(sem_t *sem) {
    kmt_spin_lock(sem->slock);
}
static void kmt_sem_signal(sem_t *sem) {
    kmt_spin_unlock(sem->slock);
}

/*---------------------------------------thread-------------------------------------------------------*/
static int  kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg) {

    task = (task_t *)pmm->alloc(sizeof(task_t));
    task->name    = name;
    task->entry   = entry;
    //task->stack   = (uint8_t *)pmm->alloc(STACK_SIZE);
    Area stack    = (Area) { &task->context + 1, task + 1 };
    task->context = kcontext(stack, task->entry, arg);
    task->next    = NULL;
    task->status  = RUNNING;

    //将所有任务加入到一个全局链表中
    Task_List *task_cur = (task_t *)pmm->alloc(sizeof(Task_List));
    task_cur->cur = task;
    task_cur->next = NULL;
    if(!task_head) {
        task_head = task_cur;
        task_read = task_head;
    }
    else {
        task_pre->next = task_cur;
    }
    task_pre = task_cur;

    return 0;
}

static void kmt_init() {
    //锁的初始化
    kmt_spin_init(splk,"spin lock");
    kmt_sem_init(semlk,"sem lock",5);
}

static void kmt_teardown(task_t *task) {
    //主要回收pmm分配的stack
    return;
}
MODULE_DEF(kmt) = {
    .init = kmt_init,
    .create = kmt_create,
    .teardown = kmt_teardown,
    .spin_init = kmt_spin_init,
    .spin_lock = kmt_spin_lock,
    .spin_unlock = kmt_spin_unlock,
    .sem_init = kmt_sem_init,
    .sem_wait = kmt_sem_wait,
    .sem_signal = kmt_sem_signal,
 // TODO
};
