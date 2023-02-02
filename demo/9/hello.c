// hello.c
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
int main();

void hello() {
//   printf("Hello World\n");
    char *p = (char *)main + 0xd + 1;
    int32_t offset = *(int32_t *)p; //为二进制文件中的相对位移，为负数
    assert((char *)main + 0x12 + offset == (char *)hello);
    printf(">>>=== *main = %p    *hello = %p offset = %d\n",main,hello,offset);
}