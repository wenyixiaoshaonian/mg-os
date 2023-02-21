#include <os.h>

#define STACK_SIZE 8192
void func(void *arg) {
  while (1) {
    lock();
    printf("Thread-%s on CPU #%d\n", arg, cpu_current());
    unlock();
    for (int volatile i = 0; i < 100000; i++) ;
  }
}

static int  kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg) {

    task->name    = name;
    task->entry   = entry;
    task->stack   = pmm->alloc(STACK_SIZE);
    Area stack    = (Area) { &task->context + 1, task + 1 };
    task->context = kcontext(stack, task->entry, arg);
    task->next    = NULL;
    return 0;
}

static void kmt_init() {
    //更具需求创建多个线程，放入一个链表
}

static void kmt_teardown(task_t *task) {
    //主要回收pmm分配的stack
    return;
}
static void kmt_spin_init(spinlock_t *lk, const char *name) {

    lk->name = name;
    lk->lock = 0;   //解锁状态
}
static void kmt_spin_lock(spinlock_t *lk) {

  while (1) {
    intptr_t value = atomic_xchg(lk, 1);
    if (value == 0) {
      break;
    }
  }
}
static void kmt_spin_unlock(spinlock_t *lk) {

    atomic_xchg(lk, 0);
}
static void kmt_sem_init(sem_t *sem, const char *name, int value) {

    sem->name = name;
    sem_t->count = value;
    sem_t->slock->lock = 0;   //解锁状态
}
static void kmt_sem_wait(sem_t *sem) {
    kmt_spin_lock(sem_t->slock);
    if(sem_t->count) {
        sem_t->count--;
    }
    else {      //没有资源
        
    }
    kmt_spin_unlock(sem_t->slock);
}
static void kmt_sem_signal(sem_t *sem) {
    kmt_spin_lock(sem_t->slock);
    sem_t->count++;
    kmt_spin_unlock(sem_t->slock);
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
