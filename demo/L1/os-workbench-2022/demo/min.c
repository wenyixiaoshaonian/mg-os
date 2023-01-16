#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>

int main()
{
    //printf("hello world.....\n");
    // while(1) {
    //     ;
    // }
    syscall(SYS_exit,42);
}