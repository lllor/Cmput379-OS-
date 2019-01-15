#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include<string.h>
#include<linux/shm.h>
int shmid; 
char *viraddr;
void tt(int signo,siginfo_t *info,void *p);
int main()
{
 
 
 struct sigaction act;
 int sig;
 pid_t pid;
 pid=getpid();
 printf("pid is %d\n",pid);
 sigemptyset(&act.sa_mask);
 act.sa_sigaction = tt;
 act.sa_flags = SA_SIGINFO;//表示信号附带的参数可以被传进来
 if(sigaction(SIGUSR1,&act,NULL)<0)
 {
  printf("sigaction error\n");
  return 0;
 }
  printf("等待接收信号---\n");
  pause();
 return 0;
}
void tt(int signo,siginfo_t *info,void *p)
{
 shmid=shmget(0x44,30,0666|IPC_CREAT);/*获取共享内存*/
 viraddr=(char*)shmat(shmid,0,0);         /*附接到进程的虚拟地址空间*/
 printf("传递进来的字符串为:\n%s\n",viraddr);  /*输出信息内容*/
 
 printf("receive SIGUSR1\n");
 printf("信号编码为%d\n",signo);
 shmdt(viraddr);                  /*断开连接*/ 
    shmctl(shmid,IPC_RMID,0);                /*撤销共享内存*/
 
}

