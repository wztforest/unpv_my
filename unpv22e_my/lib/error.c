#include "unpipc.h"

#include <stdarg.h>
#include <syslog.h>


int daemon_proc;   /*被daemon_init 设置为非零*/

static void err_doit(int,int,const char *,va_list);

/*与系统调用相关的非致命错误，打印消息并返回*/
void err_ret(const char *fmt,...)
{
    va_list ap;
    
    va_start(ap,fmt);
    err_doit(1,LOG_INFO,fmt,ap);
    va_end(ap);
    return;
}

/*与系统调用相关的致命错误，打印消息并返回*/
void err_sys(const char *fmt,...)
{
    va_list ap;

    va_start(ap,fmt);
    err_doit(1,LOG_ERR,fmt,ap);
    va_end(ap);
    exit(1);
}

/*与系统调用相关的致命错误，打印消息、转储核心并终止*/
void err_dump(const char *fmt,...)
{
    va_list ap;
    va_start(ap,fmt);
    err_doit(1,LOG_ERR,fmt,ap);
    va_end(ap);
    /*发送SIGABRT信号，
    1、此信号不能被忽略、不能被阻塞；
    2、如果注册了捕获函数，会先执行捕获函数；
    3、捕获函数执行后如果进程依然没有退出，那么恢复捕获函数为默认（终止），然后再次发送此信号给进程
    */
    abort();  
    exit(1);
}

/*与系统调用无关的非致命错误，打印消息并返回*/
void err_msg(const char *fmt,...)
{
    va_list ap;
    va_start(ap,fmt);
    err_doit(0,LOG_INFO,fmt,ap);
    va_end(ap);
    return ;
}

/*与系统调用无关的致命错误，打印消息并终止*/
void err_quit(const char *fmt,...)
{
    va_list ap;
    va_start(ap,fmt);
    va_doit(0,LOG_ERR,fmt,ap);
    va_end(ap);
    exit(1);
}

static void err_doit(int errnoflag,int level,const char *fmt,va_list ap)
{
    int errno_save, n;
    char buf[MAXLINE];

    errno_save = errno;
#ifdef HAVE_VSNPRINTF
    vsnprintf(buf,sizeof(buf),fmt,ap);  /*安全，拷贝sizeof(buf)-1*/
#else
    vsprintf(buf,fmt,ap);   /*不安全*/
#endif
    n = strlen(buf);
    if(errnoflag)
        snprintf(buf+n,sizeof(buf)-n,": %s",strerror(errno_save));
    
    strcat(buf,"\n"); /*dest最后的结束符会被覆盖掉，并在连接后的尾部再增加一个结束符*/

    if(daemon_proc)
    {
        syslog(level,buf);
    }else
    {
        fflush(stdout); /*如果stdout 和 stderr 相同*/
        fputs(buf,stderr);
        fflush(stderr);
    }
    return;
}
