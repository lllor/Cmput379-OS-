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



Squeue *buffer=NULL;                //a queue variable to stored the recieved bits
Squeue *message=NULL;               //a queue variable to stored the recieved message
int counter = 0;                    //an int variable to indicated how mush bits we already recievd == the length of buffer
struct timeval tv;                  
long lastTime = 0;                  //store the last time we recieved a signal 
long flag_time = 0;         
int flag_handler = 0;               //=1 if we already print a '?' ; =0 if we not
int end_flag = 0;


void convert(char a, char *result){
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
        usleep(100000);                               //give sometime to finish privious sending/recieving
        for(int i = 0; i < length; i++) {           //for each character, convert them to a 8 length array -8bits
            char sendChar[8];
            convert(message[i], sendChar);

            for(int a = 0; a < 8; a++) {            //for each bit, if it is '1' then simply send SIGUSR1.
                if(sendChar[a] == '1') {
                    kill(pid, SIGUSR1);
                    usleep(5000);//50000//5000      //delay the program execution for 2500us
                }
                else {
                    kill(pid, SIGUSR1);             //for each bit, if it is '0', then send a SIGUSR1
                    usleep(2000);//5000//500         //delay the program execution for 250us
                    kill(pid, SIGUSR1);             //send a SIGUSR1 again
                    usleep(3000);//45000//4500      //delay the program execution for 2250us
                }
            }                                       
        }

        for(int start = 0; start < 9; start++) {    //after sending the message content, send 8more SIGUSR1 as
            kill(pid, SIGUSR1);                     //the flag, which help handler to detect the error of sending 
      
            usleep(5000);//50000//5000              //for each signal, delay for 2500us
        }
        usleep(100000);//10000                        //delay 5000us(twice than normal send) 
        //sleep(0.01);
        kill(pid,SIGUSR1);                          //send a single SIGUSR1 as the end mark.
        //printf("send over\n");
    }


    void handler(int signal_val) {
  	
        gettimeofday(&tv, NULL);                    //update the time
    
        if(lastTime == 0) {                         //if the first time invoke the handler
            lastTime = tv.tv_sec * 1000000 + tv.tv_usec;    //update the lasttime
            addBack(buffer, '1');                   //append '1' to the buffer to avoid bug
             //counter++;
        }
        else {
            long timeNow = tv.tv_sec * 1000000 + tv.tv_usec;    //else, get the current time
            if(timeNow - lastTime < 2500) {//10000               //comput the difference between current time and the lasttime we recieved a signal, if the difference is smaller than 500, means we recieved a '0'
            leaveBack(buffer);                                  //remove the last '1'
            addBack(buffer, '0');                               //add a '0'
            }
            else if(timeNow -lastTime > 80000){                  //if the difference is greater than 5000,means we recieved a end mark
                //printf("%d",counter);
                if(counter%8!=0){                               //if the counter%8!=0, means we lose some bits,error occured
                    if (flag_handler == 0){                     //if we never detect an error occured, print "?"
                        printf("? ");
                    }
                flag_handler = 1;
                }
        
                if(flag_handler == 0){                          //if we never detect an error occured, print '!'
                    printf("! ");
                }
                                          
                print(message,'f','!');                          //print the message we recieved  
                printf("\n");           
                
                nuke(buffer);                                   //clear the queue
                nuke(message);
                flag_handler = 0;                               //reset the error_flag
                lastTime = 0;                                   //reset the time_flag
                counter = 0;                                    //reset the count_flag
                return;
            }
            else{
                addBack(buffer, '1');                           //otherwise means we recieved a 1
                counter++;                                      //counter+1
                lastTime = timeNow;
                //
            }
            
            
        }
    
        if(counter>7){                                          //once we recieved more than 8bits
            //print(buffer,'f','!');
            //printf("\n");
            counter-=8;                                         //decrease the counter
            char tempp[8];
            for(int i = 0; i < 8; i++) {
                if(!isEmpty(buffer)){                           //get the 8bits and convert them to a character
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
    
            if (tempp[0] == '1' && tempp[1] == '1' && tempp[2] == '1' && tempp[3] == '1'//if we recieved 8 consecutive 1,means no error happend
                && tempp[4] == '1' && tempp[5] == '1' && tempp[6] == '1' && tempp[7] == '1') {
            }
            else if(tempp[0] == '1') {  //no char has a tempp[0]=1, which means error happend
                if(flag_handler == 0){
                    printf("? ");
                }
                flag_handler = 1;                               //set the error_flag
            }
            else{
                addBack(message,convertBack(tempp));
            }
        }
    }



#else
    void send(char message[], int length, pid_t pid) { 
        usleep(100000);                                           //give some time to complete previous sending/recieving
        for(int i = 0; i < length; i++) {                       //for each character, convert them to a 8bit array
            char sendChar[8];
            convert(message[i], sendChar);
            for(int a = 0; a < 8; a++) {
                if(sendChar[a] == '0') {                        //for each bit, if it is 0, send SIGUSR2,else SIGUER1
                    kill(pid, SIGUSR2);
                }
                else {
                    kill(pid, SIGUSR1);
                }
                usleep(2500);//5000                             //for each sending, delay 2500us
            }
        }
        
        for(int a = 0; a<8 ; a++){                              //send 8 consecutive SIGUSR1
            kill(pid,SIGUSR1);
            usleep(2500);//5000
        }
        usleep(100000);//10000                                    //send the end mark
        kill(pid,SIGUSR1);
        return;
    }
    void handler(int signal_val) {
        gettimeofday(&tv, NULL);                
        long timeNow = tv.tv_sec * 1000000 + tv.tv_usec;        //get current time, if the first time invoke handler
        if(lastTime == 0) {                                     //set the lasttime = currenttime
            lastTime = timeNow;
        }
        //lastTime = tv.tv_sec*1000000 + tv.tv_usec;
        
        //printf("%lu\t",timeNow -lastTime);
        switch(signal_val){                                     //for each signal
            case SIGUSR1:
                if(timeNow -lastTime > 80000){//10000            //if the time difference bigger than 5000, means recieved end mark
                    
                    if(counter%8!=0){                           //if counter%8!=0, means we lose some signal
                        if (flag_handler == 0){
                            printf("? ");                       //print the error flag
                        }
                        flag_handler = 1;
                    }
        
                    if(flag_handler == 0){                      //if no error occured, print "!"
                        printf("! ");
                    }
                    
                    print(message,'f','!');                     //print the message
                    printf("\n");
                    nuke(buffer);                               //empty the queue
                    nuke(message);
                    flag_handler = 0;                           //reset the variable
                    lastTime = 0;
                    counter = 0;
                    
                    
                    return;
                }
                
                else{
                    addBack(buffer,'1');                        //if difference<5000, means we recieved a normal sigUSR1
                    lastTime = timeNow;
                    break;
                }
                break;
            case SIGUSR2:                                       //if we recieved a SIGUSR2
                addBack(buffer,'0');
                lastTime = timeNow;
                break;
        }
        counter++;
        
        if (counter%8 == 0 && counter != 0){                    //if we recieved 8bits
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
    initSqueue(&buffer);                                    //init the queue
    initSqueue(&message);

    struct sigaction sa;                                    //init the sigactiom
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_flags = SA_RESTART;

    sigaction(SIGUSR1, &sa, NULL);                          //signal handler
    sigaction(SIGUSR2, &sa, NULL);

    pid_t mypid = getpid();                                 //get pid_t
    printf("Own PID: %d\n",mypid);
    pid_t otherpid = 0;
    scanf("%d", &otherpid);
    
    if(otherpid == 0){
    	exit(1);
    }
    if(0 != kill(otherpid,0))
    {
    	exit(1);
    }
    
    char ch;
    char input[4096]={0};
    ch = getchar();

    counter = 0;   
    while(1){

        int i = 0;
        
        
        if (input[0] == 0 && isEmpty(buffer) && isEmpty(message)){//if the user havent enter anything and the process is not in send/recieving process
    	
            while((ch = getchar()) != '\n') {                   //get the user input
                input[i] = ch;
                i++;
            }
        }

        if(input[0] == '.' && strlen(input) == 1) {
            exit(1);
        }

        if(isEmpty(buffer) && isEmpty(message)){                //if the process is not in sending/recieving
        
            counter = 0;                                        //send the message
            send(input,strlen(input), otherpid);
        
            for(int i=0;i<4096;i++){                            //clean the input
                input[i] = 0;
            }
            nuke(buffer);                                       //clean the queue
            nuke(message);
            flag_handler = 0;
      
        }
     
    }
}
