/* Solaris 2.6:
   gcc -g -O2 -D_REENTRANT -Wall -D__EXTENSIONS__ test1.c -lposix4 -lpthread */
/* Digital Unix 4.0B:
   cc -g -D_REENTRANT test1.c -lrt -lpthread */

#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#define MAXNTHREADS 100
#define min(a,b)    ((a) < (b) ? (a) : (b))

int nloop;

int test;   /*测试*/

struct
{
    sem_t mutex;    /*基于内存的信号量*/
    long counter;
}shared;

void    *incr(void *),err_quit(char *);

int main(int argc,char **argv)
{
    int i,nthreads;
    pthread_t tid[MAXNTHREADS];

    if (argc != 3)
        err_quit("usage:incr_pxsem1 <#loops> <#threads>");
    /*字符串转换成整数 int型,遇到数字或正负符号才开始做转换，再遇到非数字或'\0'结束转换*/
    nloop = atoi(argv[1]);  
    nthreads = min(atoi(argv[2]),MAXNTHREADS);
    /*int sem_init(sem_t *sem,int pshared,unsigned int value)
    参数pshared为0，此信号量在所有线程可见的之间可用（如全局变量，堆上动态分配的变量等）；为1，在共享内存区域的进程间可用；
    参数value 是信号量的初始值；
    */
    if(sem_init(&shared.mutex,0,0) == -1)
        err_quit("sem_init error");
    for(i = 0;i < nthreads;i++)
    {
        if(pthread_create(&tid[i],NULL,incr,NULL) != 0)
            err_quit("pthread_create error");
    }
    
    if(sem_post(&shared.mutex) == -1)
        err_quit("sem_post error");

    for(i = 0;i < nthreads;i++)
    {
        if(pthread_join(tid[i],NULL) != 0)
            err_quit("pthread_join error");
    }
    if(shared.counter != nloop * nthreads)
        printf("error:counter = %ld\n",shared.counter);
    else
        printf("ok:counter = %ld\n",shared.counter);
    exit(0);
}

void *incr(void *arg)
{
    int i;
    for(i =0;i < nloop;i++)
    {
        /*加锁 -1
        1、shared.mutex信号量递减，递减成功立即返回，如果信号量当前的值为0，调用将阻塞，
        直到信号量上升到0以上，可以执行递减，或者信号处理程序中断调用 才能返回；
        2、sem_trywait(sem_t *sem)与sem_wait(sem_t *sem)相似，只是如果调用不能立即执行递减，调用会立即返回错误
        （errno 设置为 EAGAIN）,而不阻塞。
        3、sem_timedwait(sem_t *sem,const struct timespec*abs_timeout)与sem_wait()相似，
        只是abs_timeout指定了执行递减时间的超时时间，如果在此时间内不能执行递减，则返回失败并出现超时失败（errno 设置为ETIMEDOUT）
        所有这些函数在成功时返回0，出错时，信号量的值保存不变，返回-1，并设置errno以指示错误。*/
        if(sem_wait(&shared.mutex) == -1) 
            err_quit("sem_wait error");
        shared.counter++;
        /*解锁 +1*/
        if(sem_post(&shared.mutex) == -1)
            err_quit("sem_post error");
    }
    return NULL;
}

void err_quit(char *ptr)
{
    fprintf(stderr,"%s\n",ptr);
    exit(1);
}


