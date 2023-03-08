#include <os.h>

#define STACK_SIZE 8192

#define MAX_CPU 8

extern Task *currents[MAX_CPU];
#define current currents[cpu_current()]

extern Task_List *task_reads[MAX_CPU];
#define reads task_reads[cpu_current()]

Task_List *task_head = NULL;
Task_List *task_pre = NULL;
Task_List *task_read = NULL;
spinlock_t splk;
sem_t semlk;

void enqueue(spinlock_t *lk,Task *cur) {
    Task_List *task_cur = (task_t *)pmm->alloc(sizeof(Task_List));
    //printf(">>>=== enqueue task_cur = %p....\n",task_cur);
    task_cur->cur = cur;
    task_cur->next = NULL;
    if(!lk->waitlist_head) {
        lk->waitlist_head = task_cur;
        lk->waitlist_read = task_cur;
        //printf(">>>=== waitlist_head = %p....\n",lk->waitlist_head);
    } else {
        lk->wait_list->next = task_cur;
        if(!lk->waitlist_read) {
            lk->waitlist_read = task_cur;
            //printf(" 22 enqueue   %p \n",lk->waitlist_read);
        }
        //printf(" enqueue   %p \n",lk->waitlist_read);
    }
    lk->wait_list = task_cur;
}

Task *dequeue(spinlock_t *lk) {
    Task_List *task_cur = lk->waitlist_read;
    Task *ret = task_cur->cur;
    lk->waitlist_read = lk->waitlist_read->next;
    //printf("dequeue   %p \n",lk->waitlist_read);
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
    lk->irq_flag = 0;
    lk->wait_list = NULL;
    lk->waitlist_head = NULL;
}
static void kmt_spin_lock(spinlock_t *lk) {
    int acq = 0;
    //在中断处理程序中，不能使用睡眠的方式，不能发生任务调度
    if (lk->irq_flag) {
        //printf("222 %d\n",cpu_current());
        spin_lock(&lk->lock);
        return;
    }
    bool i = ienabled();
    iset(false);
    //在用户程序中，使用睡眠-唤醒的方式
    spin_lock(&lk->lock);
    //printf("23 %d %d\n",cpu_current(),lk->locked);
    if(lk->locked <= 0)  {
        //printf("223 %d\n",cpu_current());
        current->status = WAITTING;
        enqueue(lk, current);        //添加到等待队列
        printf("enq fin %d\n",cpu_current());
        acq = 1;
        
    } 
    else {
        lk->locked--;
    }
    spin_unlock(&lk->lock);
    if(acq) {
        printf("yield %d\n",cpu_current());
        if (i) {
            iset(true);
        }
        yield(); // 阻塞时切换
    }

}

static void kmt_spin_unlock(spinlock_t *lk) {
unlock:
    //在中断处理程序中，不能使用睡眠的方式，不能发生任务调度
    if (lk->irq_flag) {
        //printf("111 %d\n",cpu_current());
        spin_unlock(&lk->lock);
        return;
    }
    //在用户程序中，使用睡眠-唤醒的方式
    //printf("u456 %d %d\n",cpu_current(),lk->locked);
    spin_lock(&lk->lock);
    //printf("u23 %d %d\n",cpu_current(),lk->locked);
    if(lk->waitlist_read != NULL) {
        Task *task = dequeue(lk);
        task->status = RUNNING;
        //lk->locked++;
        //printf("deq fin %d\n",cpu_current());
        spin_unlock(&lk->lock);
        return;
    }
    else if(lk->locked >= lk->lock_num) {
        //printf("u56 %d  %d\n",cpu_current(),lk->locked);
        // spin_unlock(&lk->lock);
        // yield();
        // goto unlock;
    }
    else
        lk->locked++;
    //printf("u45 %d %d\n",cpu_current(),lk->locked);
    spin_unlock(&lk->lock);
}

/*---------------------------------------sem-------------------------------------------------------*/
static void kmt_sem_init(sem_t *sem, const char *name, int value) {
    sem->name = name;
    //sem_t->count = value;
    sem->slock.lock = 0;             //解锁状态
    sem->slock.locked = value;       //1时用作互斥量 解锁状态 0时作为信号量
    if(value == 1)
        sem->slock.lock_num = value;
    else
        sem->slock.lock_num = 5;       //最多允许5个信号
    sem->slock.irq_flag = 0;
    sem->slock.wait_list = NULL;
    sem->slock.waitlist_head = NULL;
}

static void kmt_sem_wait(sem_t *sem) {
    // bool i = ienabled();
    // iset(false);
    //printf(">>==33 %d \n",cpu_current());
    kmt_spin_lock(&sem->slock);
    printf(">>==44 %d \n",cpu_current());
    // if (i) {
    //     iset(true);
    // }
       
}
static void kmt_sem_signal(sem_t *sem) {
    bool i = ienabled();
    iset(false);
    //printf(" >>==11  %d \n",cpu_current());
    kmt_spin_unlock(&sem->slock);
    printf(" >>==55  %d \n",cpu_current());
    if (i) {
        iset(true);
    }
}

/*---------------------------------------thread-------------------------------------------------------*/
static int  kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg) {

    task->name    = name;
    task->entry   = entry;
    Area stack    = (Area) { &task->context + 1, task + 1 };
    task->context = kcontext(stack, task->entry, arg);
    // task->next    = NULL;
    task->status  = RUNNING;
    task->call_status   = CALLABLE;

    //将所有任务加入到一个全局链表中
    Task_List *task_cur = (task_t *)pmm->alloc(sizeof(Task_List));
    task_cur->cur = task;
    task_cur->next = NULL;
    if(!task_head) {
        task_head = task_cur;
        task_read = task_head;
        task_reads[0] = task_head;
        task_reads[1] = task_head;
        task_reads[2] = task_head;
        task_reads[3] = task_head;
    }
    else {
        task_pre->next = task_cur;
    }
    task_pre = task_cur;
    return 0;
}

int locked = 0;
void lock()   { while (atomic_xchg(&locked, 1)); }
void unlock() { atomic_xchg(&locked, 0); }

static void producer(void *arg) {
  while(1) {
    //printf("22 %d\n",cpu_current());
    lock();
    //printf("33 %d\n",cpu_current());
    printf("%s  %d\n",current->name,cpu_current());
    
    //printf("%d ",semlk->slock->locked);
    //printf("22 %d\n",semlk.slock.irq_flag);
    //kmt_sem_signal(&semlk);
    //printf("44 %d\n",cpu_current());
    unlock();
    
    //printf("(");
    for (int volatile i = 0; i < 10000; i++) ;
  }
  
  }

static void consumer(void *arg) {
  while(1) {
    //printf("00 %d\n",cpu_current());
    lock();
    //printf("11 %d\n",cpu_current());
    printf("%s  %d\n",current->name,cpu_current());
    
//    printf("22 %d\n",cpu_current());
    //printf("11 %d\n",semlk.slock.irq_flag);
    //kmt_sem_wait(&semlk);
    //printf("55 %d\n",cpu_current());
    unlock();
    
    //printf(")");
    for (int volatile i = 0; i < 10000; i++) ;
  }
}

static void kmt_init() {
    //锁的初始化
    kmt_spin_init(&splk,"spin lock");
    splk.irq_flag = 1;    //该互斥锁仅在中断使用 
    kmt_sem_init(&semlk,"sem lock",0);

    // kmt_create(pmm->alloc(sizeof(task_t)),
    //           "main", NULL, NULL);

    kmt_create(pmm->alloc(sizeof(task_t)),
              "thread-1", producer, NULL);
    kmt_create(pmm->alloc(sizeof(task_t)),
              "thread-2", consumer, NULL);
    kmt_create(pmm->alloc(sizeof(task_t)),
              "thread-3", producer, NULL);
    kmt_create(pmm->alloc(sizeof(task_t)),
              "thread-4", consumer, NULL);

//   kmt_create(pmm->alloc(sizeof(task_t)),
//               "thread-5", consumer, NULL);
//   kmt_create(pmm->alloc(sizeof(task_t)),
//               "thread-6", consumer, NULL);
//   kmt_create(pmm->alloc(sizeof(task_t)),
//               "thread-7", consumer, NULL);
//   kmt_create(pmm->alloc(sizeof(task_t)),
//               "thread-8", consumer, NULL);
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
