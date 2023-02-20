#include <kernel.h>
#include <klib.h>
#include <klib-macros.h>

// Spinlock
typedef int spinlock_p;

void spin_lock(spinlock_p *lk);
void spin_unlock(spinlock_p *lk);

void *kalloc(size_t size);
void kfree(void *ptr);