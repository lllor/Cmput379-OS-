/*html:blog.51cto.com/10638473/1983100
operate: 
gcc -o test4 oneprocess.c
./test4
(show process PID)
123456(doesnt matter, must integer)
(Show get a Signal)
**why enter a new line again will not send a SIGUSR1 to current process
**but SIGINT works fine
*/
#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include <unistd.h>

#ifdef SIGNAL//user2
#else//user1
char* GetInput(){
    char temp;
    int i=0,c;
    temp = getchar();
    //temp = getchar();
    
    while (temp!='.')
    {
        if (temp == '!'){
            char pMsg[2048] = {0};
            i = 0;
            printf("valid send message\n");
            temp = getchar();
            while (i<2047 && (c=getchar())!='\n'){
                pMsg[i++] = c;
            }
            pMsg[i] = 0;
            
            
            //temp = getchar();
            
        }
    
        else{
            printf("invalid sending message\n");
            exit(1);
        }
    }
}
#endif
//deal with SIGUSR1
void signalDeal(int signo,siginfo_t *info,void *context)
{
    char *pMsg=(char*)info->si_value.sival_ptr;
    printf("recieved signal:%d\n", signo);
    printf("recieved message:%s\n", pMsg);
}
//SIGINT
void testSignal(int sig)
{
    printf("Get a signal %d\n",sig);
    
}
//SIGINT
void testSignal2(int sig)
{
    printf("Get a user defined signal %d\n",sig);
    
}

//main function
int main(int argc,char *argv[])
{
    struct sigaction sigAct; //init a sigaction
    sigAct.sa_flags = SA_SIGINFO;//format?
    sigAct.sa_sigaction = signalDeal;

    if(sigaction(SIGUSR1,&sigAct,NULL)==-1)
    {
        printf("sigaction failed!\n");
        exit(1);
    }
    sigval_t val;
    char pMsg[2048]="Hello worls";
    val.sival_ptr = pMsg;
    
    
    int other= -1;
    printf("Own PID:%d\n",getpid());
    
    while (other == -1)
    {
        scanf("%d",&other);

    }
    
    wait(NULL);
    if(sigqueue(other,SIGUSR1,val) == -1)
    {
        printf("sigqueue faild!\n");
        exit(1);
    }
    char temp = getchar();


    if(signal(SIGINT,testSignal)== SIG_ERR){
        printf("usr1 error");
    }
    //if(signal(SIGUSR1,signalDeal)==SIG_ERR){
    //    printf("usr2 error");
    //}
    if(sigqueue(other,SIGUSR1,val) == -1)
    {
        printf("sigqueue faild!\n");
        exit(1);
    }

    //kill(getpid(),SIGINT);
    //kill(getpid(),SIGUSR1);

    
    
    

    
   

    val.sival_ptr = pMsg;
        
    if(sigqueue(other,SIGUSR1,val) == -1)
    {
        printf("sigqueue faild!\n");
        exit(1);
    }
    


    if(sigqueue(other,SIGUSR1,val) == -1)
    {
        printf("sigqueue faild!\n");
        exit(1);
    }

    


    while (1)
    {
        sleep(1);
    }
    return 0;
}
