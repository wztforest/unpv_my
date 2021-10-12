#ifndef __unpipc_h
#define __unpipc_h

#include "../config.h" /*当前操作系统的配置选项，由配置生成*/

#include <sys/types.h>  /*基本系统数据类型*/
#include <sys/time.h>   /*timeval{} for select()*/
#include <time.h>       /* timespec{} for pselect() */
#include <errno.h>
#include <fcntl.h>      /*非阻塞使用*/ 
#include <limits.h>     /*pepe_buf*/
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>   /*用于 S_xxx 文件模式常量*/
#include <unistd.h>
#include <sys/wait.h>

#ifdef HAVE_MQUEUE_H
#include <mqueue.h>     /*posix 消息队列*/
#endif

#ifdef HAVE_SEMAPHORE_H
#include <semaphore.h>  /*posix 信号量*/

#ifndef SEM_FAILED
#define SEM_FAILED  ((sem_t *)(-1))
#endif

#endif

#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>   /*posix 共享内存*/
#endif

#ifdef MAP_FAILED
#define MAP_FAILED  ((void *)(-1))
#endif

#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>    /*System V IPC*/
#endif

#ifdef HAVE_SYS_MSG_H
#include <sys/msg.h>    /*system v message queues*/
#endif

#ifdef HAVE_SYS_SEM_H
#ifdef __bsdi__
#undef HAVE_SYS_SEM_H   /* hack: BSDI's semctl() prototype is wrong */
#else
#include <sys/sem.h>    /*system v 信号量*/
#endif

#ifndef HAVE_SEMUN_UNION
union semun{    /*define union for semctl()*/
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}
#endif
#endif  /* HAVE_SYS_SEM_H */




#endif /* end of __unpipc.h*/