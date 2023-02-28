#include <kernel.h>
#include <klib.h>

int main() {
  ioe_init();
  cte_init(os->trap);
  os->init();
  
  printf(">>>===os_init end  %d \n",cpu_count());
  iset(true);
  mpe_init(os->run);
  return 1;
}
