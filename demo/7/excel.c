#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    execlp("echo","echo","hello","os",NULL);
}