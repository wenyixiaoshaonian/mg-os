#include <os.h>

#define STACK_SIZE 8192

void enqueue(Task_List *list,Task *cur) {
    //todo
}

Task *dequeue(Task_List *list) {
    //todo
}
void func(void *arg) {
  while (1) {
    lock();
    printf("Thread-%s on CPU #%d\n", arg, cpu_current());
    unlock();
    for (int volatile i = 0; i < 100000; i++) ;
  }
}
/*---------------------------------------thread-------------------------------------------------------*/
static int  kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg) {

    task->name    = name;
    task->entry   = entry;
    task->stack   = pmm->alloc(STACK_SIZE);
    Area stack    = (Area) { &task->context + 1, task + 1 };
    task->context = kcontext(stack, task->entry, arg);
    task->next    = NULL;
    task->status  = RUNNING;
    return 0;
}

static void kmt_init() {
    //创建一个链表头 锁的初始状态
}

static void kmt_teardown(task_t *task) {
    //主要回收pmm分配的stack
    return;
}
/*---------------------------------------spin-------------------------------------------------------*/
static void spin_lock(int *lock) {

  while (1) {
    intptr_t value = atomic_xchg(lock, 1);
    if (value == 0)
        break;
  }
}

static void spin_unlock(int *lock) {
    atomic_xchg(lock, 0);
}
/*---------------------------------------metux-------------------------------------------------------*/
static void kmt_spin_init(spinlock_t *lk, const char *name) {
    lk->name = name;
    lk->lock = 0;       //解锁状态
    lk->locked = 0;     //解锁状态
    lk->lock_num = 1;   //当作互斥锁使用
    lk->wait_list = NULL;
}
static void kmt_spin_lock(spinlock_t *lk) {
    int acq = 0;
    spin_lock(lk->lock);
    if(lk->locked >= lk->lock_num) {
        current.status = WAITTING;
        enqueue(lk->wait_list, current);        //添加到等待队列
    }
    else {
        lk->locked ++;
        acq = 1;
    }
    spin_unlock(lk->lock);
    if(acq)
        yield(); // 阻塞时切换
}

static void kmt_spin_unlock(spinlock_t *lk) {
    spin_lock(lk->lock);
    if(lk->wait_list) {
        Task *task = dequeue(lk->wait_list);
        task->status = RUNNING;
    }
    else if(lk->locked -1  >= 0) 
        lk->locked --;
    else
        lk->locked = 0;
    spin_unlock(lk->lock);
}

/*---------------------------------------sem-------------------------------------------------------*/
static void kmt_sem_init(sem_t *sem, const char *name, int value) {

    sem->name = name;
    //sem_t->count = value;
    sem_t->slock->lock = 0;             //解锁状态
    sem_t->slock->locked = 0;           //解锁状态
    sem_t->slock->lock_num = value;     //当作信号量使用
    sem_t->slock->wait_list = NULL;
}

static void kmt_sem_wait(sem_t *sem) {
    kmt_spin_lock(sem_t->slock);
}
static void kmt_sem_signal(sem_t *sem) {
    kmt_spin_unlock(sem_t->slock);
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
