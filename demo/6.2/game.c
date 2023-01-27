#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int a;
    a = 9998;
    while(1) {
        printf("a = %d \n",a);
        sleep(1);
    }
    return 0 ;
}