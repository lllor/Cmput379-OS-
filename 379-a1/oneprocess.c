/*html:blog.51cto.com/10638473/1983100
使用sigqueue函数向进程自身发送一个SIGUSR1信号，
并获取该信号的信号值
*/
#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include <unistd.h>
//SIGUSR1的处理函数
void signalDeal(int signo,siginfo_t *info,void *context)
{
    char *pMsg=(char*)info->si_value.sival_ptr;
    printf("接收到的信号标号是:%d\n", signo);
    printf("接收到信息:%s\n", pMsg);
}
//主函数
int main(int argc,char *argv[])
{
    struct sigaction sigAct;
    sigAct.sa_flags = SA_SIGINFO;
    sigAct.sa_sigaction = signalDeal;
    
    if(sigaction(SIGUSR1,&sigAct,NULL)==-1)
    {
        printf("sigaction函数调用失败!\n");
        exit(1);
    }
    sigval_t val;
    
    char pMsg[2048];
    char temp;
    temp = getchar();
    int i=0,c;
    while (temp!='.')
    {
        if (temp == '!'){
            
            //printf("valid send message\n");
            temp = getchar();
            while (i<2047 && (c=getchar())!='\n'){
                pMsg[i++] = c;
            }
            pMsg[i] = 0;
            
            val.sival_ptr = pMsg;
            
            if(sigqueue(getpid(),SIGUSR1,val) == -1)
            {
                printf("sigqueue faild!\n");
                exit(1);
            }
            
            temp = getchar();
	    for ( i=0;i<2048;i++ ) pMsg[i]=0;
        }
	
        else{
            printf("invalid sending message\n");
	    temp = getchar();
            for ( i=0;i<2047;i++ ) pMsg[i]=0;
        }
    }







    
    
    return 0;
}
