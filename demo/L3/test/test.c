#include <stdio.h>
#include <stddef.h>

#define STUD(_) \
    _(0, 26, "xiaoming",1) \
    _(1, 22, "xiaohua",0) \
    _(2, 21, "jianjun",1) \
    _(3, 24, "aiguo",1) \

#define INIT(id,age,name,sex) \
    student[id] = student_create(id,age,name,sex); \

#define TEST 10

struct stud
{
  size_t id;
  size_t age;
  wchar_t *name;
  size_t se;
} *student[4];

typedef struct
{
    char LCD_COM0 :1;
    char LCD_COM1 :1;
    char LCD_COM2 :1;
    int LCD_COM3 :1;
    char LCD_COM4 :1;
    char LCD_COM5 :1;
    char LCD_COM6 :1;
    char LCD_COM7 :1;   
}LcdCom;

struct stud* student_create(size_t id,size_t age,wchar_t *name,size_t se) {
    struct stud* student = malloc(sizeof(struct stud));
    student->id = id;
    student->age = age;
    student->name =  malloc(20 *sizeof(wchar_t));
    student->name = name;
    student->se = se;
    return student;
}
int main() {
    STUD(INIT);
    for(int i=0;i<4;i++) {
       printf(">>==11 %d %d %s %d\n",student[i]->id,student[i]->age,student[i]->name,student[i]->se); 
    }
    printf(">>= %d \n",sizeof(LcdCom));
    int a = TEST;
    return 0;
}