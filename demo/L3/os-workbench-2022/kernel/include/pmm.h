//#include <common.h>

#define USE 1
#define FREE 0

#define HEADSIZE sizeof(h_block)

#define SPIN_INIT(lk) \
  *lk = 0;

#define SPIN_LOCK(lk) \
  while (1) { \
    intptr_t value = atomic_xchg(lk, 1); \
    if (value == 0) break;}

#define SPIN_UNLOCK(lk) \
  atomic_xchg(lk, 0);


typedef int spinlock_p;
typedef struct x_block h_block;

struct x_block {  // 40 bite
  size_t size;    // 8 bite
  int status;     // 4 bite
  void *adr;      // 8 bite
  h_block *next;  // 8 bit
  int use_flag;    // 4 bit
};


typedef struct {
    spinlock_p slock;
    size_t len;
    h_block *head;
    h_block *pre;
    size_t used_len;
} pmm_body;