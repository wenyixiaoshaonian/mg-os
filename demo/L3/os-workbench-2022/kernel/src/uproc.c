#include <os.h>
#include <syscall.h>

#include "initcode.inc"

static void uproc_init() {
    vme_init(pmm->alloc, pmm->free);
}

static int uproc_kputc(task_t *task, char ch) {
    
    return 0;
}

static int uproc_fork(task_t *task) {

    return 0;
}

static int uproc_wait(task_t *task, int *status) {

    return 0;
}

static int uproc_exit(task_t *task, int status) {

    return 0;
}

static int uproc_kill(task_t *task, int pid) {

    return 0;
}

static void *uproc_mmap(task_t *task, void *addr, int length, int prot, int flags) {

    return NULL;
}

static int uproc_getpid(task_t *task) {

    return 0;
}

static int uproc_sleep(task_t *task, int seconds) {

    return 0;
}

static int64_t uproc_uptime(task_t *task) {

    return 0;
}

MODULE_DEF(uproc) = {
    .init = uproc_init,
    .kputc = uproc_kputc,
    .fork = uproc_fork,
    .wait = uproc_wait,
    .exit = uproc_exit,
    .kill = uproc_kill,
    .mmap = uproc_mmap,
    .getpid = uproc_getpid,
    .sleep = uproc_sleep,
    .uptime = uproc_uptime,
};
