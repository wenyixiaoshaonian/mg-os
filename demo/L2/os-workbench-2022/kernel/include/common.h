#include <kernel.h>
#include <klib.h>
#include <klib-macros.h>


typedef int spinlock_p;
typedef struct irq_h irq_handle;
typedef struct task Task;
typedef struct task_List Task_List;

enum os_status
{
  RUNNING = 1,
  WAITTING,
};

struct irq_h{
  int seq;
  int event;
  handler_t handler;
  struct irq_h *next;
};

struct task {
  struct {
    int status;
    const char *name;
    struct task *next;
    void      (*entry)(void *);
    Context    *context;
  };
  uint8_t stack[8192];
};
