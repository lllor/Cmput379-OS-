#include <signal.h> 
#include <stdio.h> /*响应SIGINT信号*/ 
#include <unistd.h>
#include <stdlib.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////
/*void sig_usr(int signo){ 
  pid_t pid;
  if(signo==SIGINT) 
  printf("received SIGINT\n"); 
  kill(pid,SIGKILL); /*可以使用raise(SIGKILL);*/ //} /*主程序中捕捉SIGINT信号*/ 
/*int main(){ 
  if(signal(SIGINT,sig_usr)==SIG_ERR) 
    perror("error"); 
  while(1); 
  return 0; 
}*/
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

/*对信号SIGINT的响应函数*/
/*void sig_int(int sig)
{
    printf("Get signal: %d is ", sig);//SIGINT == 2(value)
    printf("received\n");
    /*重新注册信号*/
/*    (void) signal(SIGINT, sig_int);
}

int main()
{
    (void) signal(SIGINT, sig_int);

    while(1) 
    {
        printf("Hello World!\n");
        sleep(1);
    }
    return 0;
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/*对信号SIGINT的响应函数*/ 
/*void sig_int(int sig)
{ 
  printf("Get signal: %d\n", sig); //doesnt need to reinit the signal
} 
int main()
{ 
/*初始化sigaction 结构*/ 
/*  struct sigaction act; 
  act.sa_handler = sig_int; 

  sigemptyset(&act.sa_mask); 
  act.sa_flags = 0; 
  sigaction(SIGINT, &act, 0); //correspond to int sigqueue(pid_t pid,int sig,const union sigval value);
  while(1) { 
    printf("Hello World!\n"); 
    sleep(1); 
  } 
}*/
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <signal.h> #include <stdio.h> #include <stdlib.h> static void sig_quit(int signo)
//{ printf("caught SIGQUIT\n"); if (signal(SIGQUIT, SIG_DFL) == SIG_ERR) perror("can't reset SIGQUIT"); } static void sig_int(int signo)
//{ printf("caught SIGINT\n"); if (signal(SIGINT, SIG_DFL) == SIG_ERR) perror("can't reset SIGINT"); } int main(void)
//{ sigset_t  newmask, oldmask, pendmask; if (signal(SIGQUIT, sig_quit) == SIG_ERR) perror("can't catch SIGQUIT"); if (signal(SIGINT, sig_int) == SIG_ERR) perror("can't catch SIGINT"); sigemptyset(&newmask); /* 添加信号SIG_BLOCK和SIGINT至信号集*/ sigaddset(&newmask, SIGQUIT); sigaddset(&newmask, SIGINT); /* 设置为屏蔽这两个信号并保存当前的信号屏蔽字 */ if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) perror("SIG_BLOCK error"); sleep(5);/*进程休眠期间将阻塞的信号放置信号阻塞队列中*/ if (sigpending(&pendmask) < 0) perror("sigpending error"); /*处理信号阻塞队列*/ if (sigismember(&pendmask, SIGQUIT)) printf("SIGQUIT pending\n"); if (sigismember(&pendmask, SIGINT)) printf("SIGINT pending\n"); /* 恢复最初的信号屏蔽字 */ if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) perror("SIG_SETMASK error"); fprintf(stderr,"SIGNAL unblocked\n"); sleep(5); exit(0); }

/////////////////////////////////////////////////////////////////////////////////////////////////////
static void sigHandler_sigusr1(int sig)
{
    //sig contains the signal number that was received
    printf("Caught SIGUSR1, %d\n", getpid());
    //kill(getpid(), SIGUSR1);

}

static void sigHandler_sigusr2(int sig)
{
    //sig contains the signal number that was received
    printf("Caught SIGSR2, %d\n", getpid());
    //kill(getpid(), SIGUSR2);
}

static void sigHandler_sigint(int sig)
{
    //sig contains the signal number that was received
    printf("Caught SIGINT, Existing, %d\n", getpid());
    //kill(getpid(), SIGINT);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{

    if (signal(SIGUSR1, sigHandler_sigusr1) == SIG_ERR)
        printf("Unable to create handler for SIGUSR1\n");

    if (signal(SIGUSR2, sigHandler_sigusr2) == SIG_ERR)
        printf("Unable to create handler for SIGUSR2\n");

    if (signal(SIGINT, sigHandler_sigint) == SIG_ERR)
        printf("Unable to create handler for SIGINT\n");

    kill(getpid(), SIGUSR1);
    //(void) signal(SIGUSR1, sigHandler_sigusr1);
    kill(getpid(), SIGUSR2);
    //kill(getpid(), SIGINT);

    

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/*
int main(int argc, char* const argv[])
{
        struct sigaction act;
        act.sa_sigaction = sig_action;
        act.sa_flags = SA_SIGINFO;
        pid_t pid;
        if ((pid = fork()) == -1)
        {
                return 1;
        }
        else if (pid == 0)
        {
         sleep(1);
            union sigval val;
            val.sival_ptr = "hello world";
            sigqueue(getppid(), SIGUSR1, val);
            exit(0);
        }
        else
        {
         sigaction(SIGUSR1, &act, NULL);
         wait(NULL);
        }
        return 0;
 }
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
void myFun(int,siginfo_t*,void *p);
/*
int main(int argc,char* argv[])
{
  struct sigaction act;
  int sig;
  pid_t pid;
  pid = getpid();
  printf("pid is: %d\n",pid);
/*
  act.sa_sigaction = myFun;
  act.sa_flags = SA_SIGINFO;

  if(sigaction(SIGRTMIN,&act,NULL)<0)
  {
    printf("install sig error\n");
    return 0;
  }

  while(1){
    sleep(2);
    printf("wait for the signal\n");
  }

} 
void sig_action(int signo, siginfo_t *info, void *addr)
 {
  printf("%s\n", (char*)info->si_value.sival_ptr);
 }
*/




