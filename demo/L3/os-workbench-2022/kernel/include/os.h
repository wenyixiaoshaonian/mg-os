//#include <common.h>

extern Task *currents[MAX_CPU];
#define current currents[cpu_current()]
#define MAXBLOCK 16384   //16*1024

enum ops { OP_ALLOC = 0, OP_FREE };

struct malloc_op {
  enum ops type;
  size_t sz; 
  void *addr; 
};
