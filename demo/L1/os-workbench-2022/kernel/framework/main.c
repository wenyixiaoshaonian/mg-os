#include <kernel.h>
#include <klib.h>

int mains() {
  os->init();
  printf(">>>===1111 cpu_count = %d\n",cpu_count());
  mpe_init(os->run);
  return 1;
}
