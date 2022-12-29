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
int tree() 
{
    DIR * dir;
    DIR * dir2;
    struct dirent * ptr;
    struct dirent * ptr2;
    dir = opendir("/proc");
    char name[64] = "/proc/";
    char cmd[512] = { 0 };
    char buff[512] = { 0 };
    char name2[512] = { 0 };
    FILE *file = NULL;
    struct pro_info *head = NULL;
    struct pro_info *cur = NULL;
    char val[10] = { 0 };
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
                    // printf(">>>===111 info->name = %s", info->name);

                    sprintf(cmd, "cat %s/%s | grep %s | grep -v grep | awk '{print $2}'",name2,ptr2->d_name,"Pid");
                    file = popen(cmd, "r");
                    if (!file) {
                        printf("cannot open it\n");
                        return -1;
                    }
                    fgets(val, sizeof(int)*2, file);
                    info->pid = atoi(val);
                    // printf(" info->Pid = %ld", info->pid);
                    sprintf(cmd, "cat %s/%s | grep %s | grep -v grep | awk '{print $2}'",name2,ptr2->d_name,"PPid");
                    file = popen(cmd, "r");
                    if (!file) {
                        printf("cannot open it\n");
                        return -1;
                    }
                    fgets(val, sizeof(int)*2, file);
                    info->ppid = atoi(val);
                    // printf(" info->PPid = %ld \n", info->ppid);
                }
            }
            strcpy(name2,name);
        }
    }
    cur = head;
    do {
        printf(">>>===111 info->name = %s  info->Pid = %ld  info->PPid = %ld \n", cur->name,cur->pid,cur->ppid);
        cur = cur->next;
    }while(cur);

    closedir(dir);

    return 0;
}
