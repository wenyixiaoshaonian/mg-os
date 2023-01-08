#include <kernel.h>
#include <klib.h>
#include <klib-macros.h>

// Spinlock
typedef int spinlock_t;

void spin_lock(spinlock_t *lk);
void spin_unlock(spinlock_t *lk);

void *kalloc(size_t size);
void kfree(void *ptr);