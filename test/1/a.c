#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

int main() {

	// printf("hello world\n");
	syscall(SYS_exit,42);
}
