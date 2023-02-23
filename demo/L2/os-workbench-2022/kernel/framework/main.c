#include <kernel.h>
#include <klib.h>

int main() {
  ioe_init();
  cte_init(os->trap);
  //printf(">>>===os_init start\n");
  os->init();
  printf(">>>===os_init end\n");
  mpe_init(os->run);
  return 1;
}
