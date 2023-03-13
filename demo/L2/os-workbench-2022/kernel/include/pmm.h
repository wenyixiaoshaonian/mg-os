#include <common.h>

#define USE 1
#define FREE 0

#define HEADSIZE sizeof(h_block)

typedef int spinlock_p;
typedef struct x_block h_block;

struct x_block {  // 40 bite
  size_t size;    // 8 bite
  int status;     // 4 bite
  void *adr;      // 8 bite
  h_block *next;  // 8 bit
  int use_flag;    // 4 bit
};


struct pmm_body {
    spinlock_p slock;
    size_t len;
    h_block *head;
    h_block *pre;
    size_t used_len;
} pmm_h;