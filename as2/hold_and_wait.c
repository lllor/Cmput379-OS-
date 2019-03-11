#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define BUFSIZE 1000000
char buf1[BUFSIZE];
char buf2[BUFSIZE];

char * msg1 = "This is a message for buf1. It is not a long string, but "
    "long enough.\n";
char * msg2 = "This is a message for buf2. It is not a long string, but "
    "long enough.\n";

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

int fib(int n){
    if (n == 0 || n == 1)
        return 1;
    return fib(n-1) + fib(n-2);
}

void * thread1(void * arg){
    pthread_mutex_lock(&mutex1);
    strcat(buf1, msg1);
	fib(35);
    
    pthread_mutex_lock(&mutex2);
    strcat(buf2, msg2);
    
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    pthread_exit(NULL);
}

void * thread2(void * arg){
    pthread_mutex_lock(&mutex2);
    strcat(buf2, msg2);
    fib(35);
    pthread_mutex_lock(&mutex1);
    strcat(buf1, msg1);
    
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    pthread_exit(NULL);
}

int main(){
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    
    int numThreads = 20;
    pthread_t thread1IDs[numThreads];
    pthread_t thread2IDs[numThreads];
    
    for (int i = 0; i < numThreads; i++){
        pthread_create(&thread1IDs[i], NULL, thread1, NULL);
        pthread_create(&thread2IDs[i], NULL, thread2, NULL);
    }
    
    for (int i = 0; i < numThreads; i++){
        pthread_join(thread1IDs[i], NULL);
        pthread_join(thread2IDs[i], NULL);
    }
    printf("buf1: %s", buf1);
    printf("buf2: %s", buf2);

    return 0;
}
