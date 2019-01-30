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
        usleep(5000);
        for(int i = 0; i < length; i++) {
            char sendChar[8];
            convert(message[i], sendChar);

            for(int a = 0; a < 8; a++) {
                if(sendChar[a] == '1') {
                    kill(pid, SIGUSR1);
                    usleep(2500);//50000//5000
                }
                else {
                    kill(pid, SIGUSR1);
                    usleep(250);//5000//500
                    kill(pid, SIGUSR1);
                    usleep(2250);//45000//4500
                }
            }
        }

        for(int start = 0; start < 9; start++) {
            kill(pid, SIGUSR1);
      
            usleep(2500);//50000//5000
        }
        usleep(5000);//10000
        //sleep(0.01);
        kill(pid,SIGUSR1);
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
            if(timeNow - lastTime < 500) {//10000
            leaveBack(buffer);
            addBack(buffer, '0');
            }
            else if(timeNow -lastTime > 5000){
                //printf("%d",counter);
                if(counter%8!=0){
                    if (flag_handler == 0){
                        printf("? ");
                    }
                flag_handler = 1;
                }
        
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
                counter = 0;
                return;
            }
            else{
                addBack(buffer, '1');
                counter++;
            }
            lastTime = timeNow;
        }
    
        if(counter>7){
            counter-=8;
            char tempp[8];
            for(int i = 0; i < 8; i++) {
                if(!isEmpty(buffer)){
                    tempp[i] = peekFront(buffer);
                    leaveFront(buffer);
                }
                else{
                    tempp[i] = '1';
                    if(flag_handler == 0){
                        printf("? ");
                    }
                    flag_handler = 1;
                }
            }
    
            if (tempp[0] == '1' && tempp[1] == '1' && tempp[2] == '1' && tempp[3] == '1'
                && tempp[4] == '1' && tempp[5] == '1' && tempp[6] == '1' && tempp[7] == '1') {
            }
            else if(tempp[0] == '1') {
                if(flag_handler == 0){
                    printf("? ");
                }
                flag_handler = 1;
            }
            else{
                addBack(message,convertBack(tempp));
            }
        }
    }



#else
    void send(char message[], int length, pid_t pid) {
        usleep(5000);
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
                usleep(2500);//5000
            }
        }
        
        for(int a = 0; a<8 ; a++){
            kill(pid,SIGUSR1);
            usleep(2500);//5000
        }
        usleep(5000);//10000
        kill(pid,SIGUSR1);
        return;
    }
    void handler(int signal_val) {
        gettimeofday(&tv, NULL);
        long timeNow = tv.tv_sec * 1000000 + tv.tv_usec;
        if(lastTime == 0) {
            lastTime = timeNow;
        }
        //lastTime = tv.tv_sec*1000000 + tv.tv_usec;
        
        //printf("%lu\t",timeNow -lastTime);
        switch(signal_val){
            case SIGUSR1:
                if(timeNow -lastTime > 5000){//10000
                    //printf("in end\n");
                    if(counter%8!=0){
                        if (flag_handler == 0){
                            printf("? ");
                        }
                        flag_handler = 1;
                    }
        
                    if(flag_handler == 0){
                        printf("! ");
                    }
                    
                    print(message,'f','!');
                    printf("\n");
                    
                    nuke(buffer);
                    nuke(message);
                    flag_handler = 0;
                    lastTime = 0;
                    counter = 0;
                    return;
                }
                
                else{
                    addBack(buffer,'1');
                    lastTime = timeNow;
                    break;
                }
                break;
            case SIGUSR2:
                addBack(buffer,'0');
                lastTime = timeNow;
                break;
        }
        counter++;
        if (counter%8 == 0 && counter != 0){
            counter -= 8;
            char temp[8];
            //print(buffer,'f','!');
            //printf("\n");
            for(int i = 0; i < 8; i++) {
                if(!isEmpty(buffer)){
                    temp[i] = peekFront(buffer);
                    leaveFront(buffer);
                }
                else{
                    temp[i] = '1';
                    if(flag_handler == 0){
                        printf("? ");
                    }
                    flag_handler = 1;
                }
            }

            if (temp[0] == '1' && temp[1] == '1' && temp[2] == '1' && temp[3] == '1'
                && temp[4] == '1' && temp[5] == '1' && temp[6] == '1' && temp[7] == '1') {
            }
            else if(temp[0] == '1') {
                if(flag_handler == 0){
                    printf("? ");
                }
                flag_handler = 1;
            }
            else{
                addBack(message,convertBack(temp));
            }
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
        //assert(isEmpty(buffer));
        //assert(isEmpty(message));
        if (input[0] == 0 && isEmpty(buffer) && isEmpty(message)){
    	
            while((ch = getchar()) != '\n') {
                input[i] = ch;
                i++;
            }
        }

        if(isEmpty(buffer) && isEmpty(message)){
        
            counter = 0;
            send(input,strlen(input), otherpid);
        
            for(int i=0;i<4096;i++){
                input[i] = 0;
            }
            nuke(buffer);
            nuke(message);
            flag_handler = 0;
      
        }
     
    }
}

