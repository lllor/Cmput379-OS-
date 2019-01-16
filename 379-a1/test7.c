#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include <unistd.h>

#ifdef SINGLE//second
void OnlineCheckReciever();
void main(){
	printf("Own PID:%d\n",getpid());
	if(signal(SIGUSR2,OnlineCheckReciever)==SIG_ERR){
        printf("usr2 error");
    }
    while(1);

}

#else//first
int getPID();
void OnlineCheckRequest(int PID);

void main(){
	printf("Own PID1:%d\n",getpid());
	int other= -1;
	//printf("Own PID:%d\n",getpid());

	while (other == -1)
	{
		printf("1");
    	scanf("%d",&other);
	}



	//int target_pid = getPID();
	printf("%d",other);
	//OnlineCheckRequest(target_pid);
	while(1);
}


#endif




void OnlineCheckRequest(int PID)//send a signal to other process to allow it get user input
{
	printf("1");
	kill(PID,SIGUSR2);
}

void OnlineCheckReciever()
{
	printf("now enter");
	//int id = getPID();
}




int getPID()//promote user enter target PID
{
	int other= -1;
	//printf("Own PID:%d\n",getpid());

	while (other == -1)
	{
		printf("1");
    	scanf("%d",&other);
	}
	
	return other;
}
