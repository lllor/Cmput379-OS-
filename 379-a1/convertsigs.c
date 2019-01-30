#define _POSIX_SOURCE
#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>
#include "squeue.h"
#include <math.h>
#include <string.h>
#include <assert.h>
#include <errno.h> //may not use



Squeue *buffer=NULL;
Squeue *message=NULL;
int counter = 0;
struct timeval tv;
long lastTime = 0;
long flag_time = 0;
int flag_handler = 0;
//int counter_single = 0;



char *convert(char a, char *result){
  int asciiNum = (int)a;
  //printf("%d\n", asciiNum);
  for(int i = 0; i < 8; i++) {
    int temp = asciiNum;
    asciiNum >>= 1;
    if(asciiNum << 1 == temp) {
      result[7-i] = '0';
    }
    else {
      result[7-i] = '1';
    }
  }
  return result;
}

char convertBack(char a[] ) { // a have to be an array with 8 element

  int result=0;
  for(int i = 7; i >= 0; i--) {
    if(a[i] == '1') {
      result += pow(2,(7-i));
    }
  }
  return (char)result;
}


#ifdef SINGLE

  void send(char message[], int length, pid_t pid) {
    for(int i = 0; i < length; i++) {
      char sendChar[8];
      convert(message[i], sendChar);

      for(int a = 0; a < 8; a++) {
        if(sendChar[a] == '1') {
          kill(pid, SIGUSR1);
          usleep(50000);//50000
        }
        else {
          kill(pid, SIGUSR1);
          usleep(5000);//5000
          kill(pid, SIGUSR1);
          usleep(45000);//45000
        }
      }
    }

    for(int start = 0; start < 9; start++) {
      kill(pid, SIGUSR1);
      usleep(50000);//50000
    }
    //printf("send over\n");
  }


  void handler(int signal_val) {
  	
    gettimeofday(&tv, NULL);
    
    if(lastTime == 0) {
      lastTime = tv.tv_sec * 1000000 + tv.tv_usec;
      addBack(buffer, '1');
      //counter++;
    }
    else {
      long timeNow = tv.tv_sec * 1000000 + tv.tv_usec;
      if(timeNow - lastTime < 10000) {//10000
        leaveBack(buffer);
        addBack(buffer, '0');
      }
      else {
        addBack(buffer, '1');
        counter++;
      }
      lastTime = timeNow;
    }

    if (counter > 7){
        counter -= 8;
        char tempp[8];
        //print(buffer,'f','!');
        //printf("\n");
        for(int i = 0; i < 8; i++) {
          tempp[i] = peekFront(buffer);
          leaveFront(buffer);
        }
        
        if (tempp[0] == '1' && tempp[1] == '1' && tempp[2] == '1' && tempp[3] == '1'
         && tempp[4] == '1' && tempp[5] == '1' && tempp[6] == '1' && tempp[7] == '1') {
          //printf("full message received");
          if(flag_handler == 0){
            printf("! ");
          }
          if(!isEmpty(message)){
            print(message,'f','!');
            printf("\n");
          }
          
          nuke(buffer);
          nuke(message);
          flag_handler = 0;
          lastTime = 0;
          return;
        }
        else if(tempp[0] == '1') {
          //printf("%d",flag_handler);
          if(flag_handler == 0){
            printf("? ");
          }
          if(!isEmpty(message)){
            print(message,'f','?');
          }
          
          flag_handler = 1;
          nuke(buffer);
          nuke(message);
          lastTime = 0;
          return;
        }
        addBack(message,convertBack(tempp));
      }
    
    
  }



#else
  void send(char message[], int length, pid_t pid) {
    for(int i = 0; i < length; i++) {
      char sendChar[8];
      convert(message[i], sendChar);
      for(int a = 0; a < 8; a++) {
        if(sendChar[a] == '0') {
          kill(pid, SIGUSR2);
        }
        else {
          kill(pid, SIGUSR1);
        }
        //sleep(1);
        usleep(50000);
      }
    }

    for(int a = 0; a<8 ; a++){
      kill(pid,SIGUSR1);
      usleep(50000);
    }
  }
  void handler(int signal_val) {
    gettimeofday(&tv, NULL);
    lastTime = tv.tv_sec*1000000 + tv.tv_usec;
  	//printf("inhandler, %d\n",counter);
    switch(signal_val){
      case SIGUSR1:
        //printf("Recieved SIGUSR1,%d\n",counter);
        addBack(buffer,'1');
        break;
      case SIGUSR2:
        //printf("Recieved SIGUSR2,%d\n",counter);
        addBack(buffer,'0');
        break;
    }
    //sleep(1);
    counter++;
    //print(buffer,'f');
    if (counter%8 == 0 && counter != 0){
      counter -= 8;
      char temp[8];
      for(int i = 0; i < 8; i++) {
        temp[i] = peekFront(buffer);
        leaveFront(buffer);
        //print(buffer,'f');
      }

      if (temp[0] == '1' && temp[1] == '1' && temp[2] == '1' && temp[3] == '1'
       && temp[4] == '1' && temp[5] == '1' && temp[6] == '1' && temp[7] == '1') {
        print(message,'f','!');
        nuke(buffer);
        nuke(message);
        return;
      }
      else if(temp[0] == '1') {
        print(message,'f','?');
        nuke(buffer);
        nuke(message);
        return;
      }

      addBack(message,convertBack(temp));

    }

  }
#endif




int main(void) {
  initSqueue(&buffer);
  initSqueue(&message);

  struct sigaction sa;
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_flags = SA_RESTART;

  sigaction(SIGUSR1, &sa, NULL);
  sigaction(SIGUSR2, &sa, NULL);

  pid_t mypid = getpid();
  printf("Own PID: %d\n",mypid);
  pid_t otherpid;
  scanf("%d", &otherpid);
  char waste = getchar();
  char input[4096]={0};


  counter = 0;
  while(1){

    int i = 0;
    char ch;
    //printf("input: ");
    assert(isEmpty(buffer));
    assert(isEmpty(message));
    if (input[0] == 0 && isEmpty(buffer) && isEmpty(message)){
    	printf("input: ");
      while((ch = getchar()) != '\n') {
          input[i] = ch;
          i++;
      }
    }
    // the program will hold at getchar(), no idea on where should
    // I put the following code
    /*
    gettimeofday(&tv, NULL);
    if(tv.tv_sec * 1000000 + tv.tv_usec - lastTime > 100000
        && (!isEmpty(buffer) || !isEmpty(message))){
      print(message, 'f', '?');
      nuke(message);
      nuke(buffer);
    }
    */

    if(isEmpty(buffer) && isEmpty(message)){
    	//printf("\nnow we can send\n");
    	counter = 0;
    	send(input,strlen(input), otherpid);
    	//usleep(10000);
    	for(int i=0;i<4096;i++){
    		input[i] = 0;
    	}
      nuke(buffer);
      nuke(message);
      flag_handler = 0;
      //printf("out\n");
    }
	   //printf("\nwe Recieved message but need time to send/or we dont recieved any message\n");
     //usleep(100000);
  }
}
