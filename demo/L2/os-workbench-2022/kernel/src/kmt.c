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

void enqueue(spinlock_t *lk,Task *cur) {
    Task_List *task_cur = (task_t *)pmm->alloc(sizeof(Task_List));
    //printf(">>>=== enqueue task_cur = %p....\n",task_cur);
    task_cur->cur = cur;
    task_cur->next = NULL;
    if(!lk->waitlist_head) {
        lk->waitlist_head = task_cur;
        //printf(">>>=== waitlist_head = %p....\n",lk->waitlist_head);

    }
    lk->wait_list = task_cur;
    lk->wait_list = lk->wait_list->next;
}

Task *dequeue(spinlock_t *lk) {
    Task_List *task_cur = lk->waitlist_head;
    Task *ret = task_cur->cur;
    lk->waitlist_head = lk->waitlist_head->next;
    //printf(">>>=== dequeue task_cur = %p....\n",task_cur);
    // pmm->free(task_cur); 
    return ret;
}

/*---------------------------------------spin-------------------------------------------------------*/
static void spin_lock(int *lock) {
    //bool i = ienabled();
    //iset(false);
    while (1) {
    intptr_t value = atomic_xchg(lock, 1);
        if (value == 0) {
            //if (i)
            //    iset(true);
            return;
        } 
    }
}

static void spin_unlock(int *lock) {
    //bool i = ienabled();
    //iset(false);
    atomic_xchg(lock, 0);
    //if (i)
    //    iset(true);
}
/*---------------------------------------metux-------------------------------------------------------*/
static void kmt_spin_init(spinlock_t *lk, const char *name) {
    
    lk->name = name;
    lk->lock = 0;       //解锁状态
    lk->locked = 1;     //解锁状态
    lk->lock_num = 1;   //当作互斥锁使用
    lk->wait_list = NULL;
    lk->waitlist_head = NULL;
}
static void kmt_spin_lock(spinlock_t *lk) {
    int acq = 0;
    spin_lock(&lk->lock);
    if(lk->locked <= 0)  {
        current->status = WAITTING;
        enqueue(lk, current);        //添加到等待队列
        acq = 1;
    } 
    else {
        lk->locked--;
        
    }
    spin_unlock(&lk->lock);

    //printf(">>>=== 222 acq = %d....\n",acq);
    if(acq) {
        printf("aa %d\n",cpu_current());
        //printf("enqueue......current->name %s\n",current->name);
        yield(); // 阻塞时切换
    }    
}

static void kmt_spin_unlock(spinlock_t *lk) {
    spin_lock(&lk->lock);
    if(lk->waitlist_head != NULL) {
        Task *task = dequeue(lk);
        task->status = RUNNING;
        printf("bb %d\n",cpu_current());
        //printf("deq %s\n",task->name);
    }
    else if(lk->locked < lk->lock_num)
        lk->locked++;

    spin_unlock(&lk->lock);
    //printf(">>>=== 111 lk->locked = %d....\n",lk->locked);
}

/*---------------------------------------sem-------------------------------------------------------*/
static void kmt_sem_init(sem_t *sem, const char *name, int value) {
    sem->name = name;
    //sem_t->count = value;
    sem->slock->lock = 0;             //解锁状态
    sem->slock->locked = 0;           //初始没有可用资源
    sem->slock->lock_num = value;     //当作信号量使用
    sem->slock->wait_list = NULL;
    sem->slock->waitlist_head = NULL;
}

static void kmt_sem_wait(sem_t *sem) {
    bool i = ienabled();
    iset(false);
    kmt_spin_lock(sem->slock);
    //printf(" %d ",sem->slock->locked);
    if (i)
       iset(true); 
}
static void kmt_sem_signal(sem_t *sem) {
    bool i = ienabled();
    iset(false);
    kmt_spin_unlock(sem->slock);
    //printf(" %d ",sem->slock->locked);
    if (i)
       iset(true); 
}

/*---------------------------------------thread-------------------------------------------------------*/
static int  kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg) {

    task->name    = name;
    task->entry   = entry;
    Area stack    = (Area) { &task->context + 1, task + 1 };
    task->context = kcontext(stack, task->entry, arg);
    // task->next    = NULL;
    task->status  = RUNNING;

    //将所有任务加入到一个全局链表中
    Task_List *task_cur = (task_t *)pmm->alloc(sizeof(Task_List));
    task_cur->cur = task;
    task_cur->next = NULL;
    if(!task_head) {
        task_head = task_cur;
        task_read = task_head;
        //printf(">>>===  task_read->cur = %p....\n",task_read->cur);
    }
    else {
        task_pre->next = task_cur;
    }
    task_pre = task_cur;
    //printf(">>>===  task->entry = %p....\n",task->entry);
    return 0;
}

static void producer(void *arg) {
  while(1) {
    printf("11 %d\n",cpu_current());
    kmt_sem_signal(semlk);
    printf("22 %d\n",cpu_current());
    //printf("(");
    for (int volatile i = 0; i < 100000; i++) ;
  }
  
  }

static void consumer(void *arg) {
  while(1) {
    printf("33 %d\n",cpu_current());
    kmt_sem_wait(semlk);
    printf("44 %d\n",cpu_current());
    //printf(")");
    for (int volatile i = 0; i < 100000; i++) ;
  }
}

static void kmt_init() {
    //锁的初始化
    splk = (spinlock_t *)pmm->alloc(sizeof(spinlock_t));
    semlk = (sem_t *)pmm->alloc(sizeof(sem_t));
    semlk->slock = (spinlock_t *)pmm->alloc(sizeof(spinlock_t));
    kmt_spin_init(splk,"spin lock");
    kmt_sem_init(semlk,"sem lock",5);

    kmt_create(pmm->alloc(sizeof(task_t)),
              "thread-1", producer, NULL);
    kmt_create(pmm->alloc(sizeof(task_t)),
              "thread-2", producer, NULL);
    kmt_create(pmm->alloc(sizeof(task_t)),
              "thread-3", producer, NULL);
    kmt_create(pmm->alloc(sizeof(task_t)),
              "thread-4", producer, NULL);

  // kmt_create(pmm->alloc(sizeof(task_t)),
  //             "thread-5", consumer, NULL);
  // kmt_create(pmm->alloc(sizeof(task_t)),
  //             "thread-6", consumer, NULL);
  // kmt_create(pmm->alloc(sizeof(task_t)),
  //             "thread-7", consumer, NULL);
  // kmt_create(pmm->alloc(sizeof(task_t)),
  //             "thread-8", consumer, NULL);
  //printf(">>>====kmt create finished\n");
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
