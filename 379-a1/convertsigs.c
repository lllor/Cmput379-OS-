#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "squeue.h"
#include <math.h>
#include <string.h>

#include <errno.h> //may not use


Squeue *buffer=NULL;
Squeue *message=NULL;
int counter = 0;



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
        if(sendChar[a] == '0') {
          kill(pid, SIGUSR1);
        }
        usleep(50000);
      }
    }
  }
  void handler(int signal_val, char biNum) {
    buffer.addBack(biNum);
    counter++;
  }


#else
  void send(char message[], int length, pid_t pid) {
    int count=0;
    //printf("%s\n",message);
    for(int i = 0; i < length; i++) {
      //printf("%c\n",message[i]);
      char sendChar[8];
      convert(message[i], sendChar);
      for(int a = 0; a < 8; a++) {
        if(sendChar[a] == '0') {
          //printf("Send Siguser2,%d\n",counter);
          kill(pid, SIGUSR2);
          //counter++;
        }
        else {
          //printf("Send siguser1,%d\n",counter);
          kill(pid, SIGUSR1);
          //counter++;
        }
        //sleep(1);
        usleep(50000);
      }
    }
    
    for(int a = 0; a<8 ; a++){
      //printf("Send siguser1 for end,%d\n",counter);
      kill(pid,SIGUSR1);
    }
    
    
    
  }
  void handler(int signal_val) {
    //printf("%d",counter);
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
  }
#endif


int receive() {
 // print(buffer,'f');
  //printf("receive\n");
  //leaveFront(buffer);
  
  
  char flag[8]={'1','1','1','1','1','1','1','1'};
  while(counter%8 == 0) {
    counter -= 8;
    char temp[8];
    
    for(int i = 0; i < 8; i++) {
      temp[i] = peekFront(buffer);
      leaveFront(buffer);
    }
    
    
    if (temp[0] == '1' && temp[1] == '1' && temp[2] == '1' && temp[3] == '1'
     && temp[4] == '1' && temp[5] == '1' && temp[6] == '1' && temp[7] == '1') {
      return 1;
    }
    addBack(message,convertBack(temp));
  }
  return 0;
}

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
  while(1){
    char input[4096]={0};
    int i = 0;
    char ch;
    
    while((ch = getchar()) != '\n') {
          input[i++] = ch;
    }
    
    //
    //printf("%s\n",input);
    
    
    
      
    send(input,strlen(input)+1, otherpid);
    if(receive()==1){
      print(message,'f');
      printf("\n");
      nuke(message);
      nuke(buffer);
    }
    
    counter = 0;
    usleep(50000);
    //exit(1);

    }
    
    
  }
  
  
  
  

