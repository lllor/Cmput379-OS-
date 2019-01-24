#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "squeue.h"
#include <math.h>

#include <errno.h> //may not use


Squeue *buffer;
Squeue *message;
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
  //printf("%s\n", result);
}

char convertBack(char[] a) { // a have to be an array with 8 element
  int result;
  for(int i = 7; i >= 0; i--) {
    if(a[i] == 1) {
      result += pow(2,i);
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
        usleep(50000);
      }
    }
  }
  void handler(int signal_val) {
    switch(signal_val){
      case SIGUSR1:
        addBack(buffer,'1');
      case SIGUSR2:
        addBack(buffer,'0');
    }
    counter++;
  }
#endif


int receive() {
  while(counter > 8) {
    counter -= 8;
    char temp[8];
    for(int i = 0; i < 8; i++) {
      temp[i] = peekFront(buffer);
    }
    if(temp == {1,1,1,1,1,1,1,1}) {
      return 1;
    }
    addBack(message,convertBack(temp));
  }
  return 0;
}

int main(void) {
  pid_t mypid = getpid();
  printf("Own PID: %d\n",mypid);
  pid_t otherpid;
  scanf("%d", &otherpid);
  send("interest",8, otherpid);
  while(1);
}
