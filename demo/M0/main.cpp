#include <stdio.h>

struct thread {
    int id, status;
};

int tree();
int add(int a,int b)
{
    return a+b;
}
int main() {
    // struct thread tpool[64], *tptr = tpool;
    // int id;
    // tptr++;
    // id = tptr - tpool + 1;
    // for(int x = 0;x < 10;)
    //     printf("Hello from thread #%c\n", "123456789ABCDEF"[x++]);

    // printf(">>>=== tpool = %d   tptr = %d  tptr -  tpool id = %d  sizeof(struct thread) = %d \n",tpool,tptr,tptr - tpool,id,sizeof(struct thread));
    // tree();
    int sum;
    sum = add(2,4);
    printf(">>>===111 sum = %d \n",sum);
    return 0;
}