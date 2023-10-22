#include <game.h>

// Operating system is a C program!
int main(const char *args) {
  ioe_init();

  puts("mainargs = \n");
  puts(args); // make run mainargs=xxx

  splash();

  puts("Press any key to see its key code...\n");
  while (1) {
    print_key();
  }
  return 0;
}
