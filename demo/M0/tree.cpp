#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "iostream"
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

using namespace std;
#define SIZE 1024
struct pro_info {
    long pid;
    long ppid;
    char name[64];
    struct pro_info *next;
};
DIR * dir;
DIR * dir2;
struct pro_info *head = NULL;

void erase_n(char *name)
{
    int i = 0;
    while(name[i] != '\0') {
        if(name[i] == '\n')
        {
            name[i] = '\0';
            return;
        }
        i++;    
    }
}

int create_info(void)
{
    struct dirent * ptr;
    struct dirent * ptr2;
    char name[64] = "/proc/";
    char cmd[512] = { 0 };
    char name2[512] = { 0 };
    FILE *file = NULL;
    struct pro_info *cur = NULL;
    char val[10] = { 0 };

    dir = opendir("/proc");
    while((ptr = readdir(dir)) != NULL)
    {
        if(ptr->d_type == 4) {
            strcpy(name2,name);
            strcat(name2,ptr->d_name);
            dir2 = opendir(name2);
            while((ptr2 = readdir(dir2)) != NULL) {
                if(!strcmp(ptr2->d_name,"status")) {
                    struct pro_info *info = (struct pro_info *)malloc(sizeof(struct pro_info));
                    if(head == NULL) {
                        head = info;
                        cur = info;
                        info->next = NULL;
                    } else {
                        cur->next = info;
                        cur = info;
                        info->next = NULL;
                    }
                    sprintf(cmd, "cat %s/%s | grep %s | grep -v grep | awk '{print $2}'",name2,ptr2->d_name,"Name");
                    file = popen(cmd, "r");
                    if (!file) {
                        printf("cannot open it\n");
                        return -1;
                    }
                    fgets(info->name, sizeof(char) *64, file);
                    erase_n(info->name);
                    sprintf(cmd, "cat %s/%s | grep %s | grep -v grep | awk '{print $2}'",name2,ptr2->d_name,"Pid");
                    file = popen(cmd, "r");
                    if (!file) {
                        printf("cannot open it\n");
                        return -1;
                    }
                    fgets(val, sizeof(int)*2, file);
                    info->pid = atoi(val);
                    sprintf(cmd, "cat %s/%s | grep %s | grep -v grep | awk '{print $2}'",name2,ptr2->d_name,"PPid");
                    file = popen(cmd, "r");
                    if (!file) {
                        printf("cannot open it\n");
                        return -1;
                    }
                    fgets(val, sizeof(int)*2, file);
                    info->ppid = atoi(val);
                }
            }
            strcpy(name2,name);
        }
    }
    return 0;
}
void show_info(struct pro_info *info)
{ 
#if 0
    int i = 0;
    do {
        printf("num : %d  info->name = %s  info->Pid = %ld  info->PPid = %ld \n", i++,info->name,info->pid,info->ppid);
        info = info->next;
    }while(info);
#else
    struct pro_info *pre;
    struct pro_info *cur = info;
    int show_ppid = 0;
    while(cur) {
        while(cur) {
            if(cur->ppid == show_ppid) {
                printf("%s----",cur->name);
                show_ppid = cur->pid;
                cur = cur->next;
                if(pre)
                    pre->next = cur->next;         //erase cur info wich is showed
                break;
            } else {
                pre = cur;
                cur = cur->next;
            }
        }
    }
    printf("\n");
#endif
}
void release_info(struct pro_info *info)
{ 
    if(info->next) {
        release_info(info->next);
    }
    free(info);
}
int tree() 
{
    int ret;
    struct pro_info *cur = NULL;
    ret = create_info();
    if(ret < 0){
        printf("create_info error... \n");
    }
    cur = head;
    show_info(cur);

    release_info(head);
    closedir(dir);
    closedir(dir2);
    
    return 0;
}
