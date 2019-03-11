/************************************************
* CMPUT 379 Winter 2019 Mutex Sample Program
* 
*   Compile: gcc mutex_toggle.c -pthread -o mutex_toggle
*   Usage: mutex_toggle [mutex]
*
*   Description: This program illustrates race conditions and mutexes
*                with pthreads in C. It starts ten threads, each of which
*                store an initial message and writes to a file, then wait, 
*                and store a second message, before writing the contents of 
*                the message to the file "thread_file" and leaving their 
*                respective threads. If a
*                command-line argument of "use_mutex" is passed in to the program,
*                a mutex is used to help synchronize the output. Try
*                running multiple times without the command line argument
*                to observe how the race condition works. Look at the code
*                and fully understand how it works. Then try with the 
*                the command line argument of "mutex" and see how the output
*                differs as you run it multiple times. Keep in mind that you
*                must look at the output file named "thread_file"
*/

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define NUM_THREADS 10

// Global mutex variable
pthread_mutex_t mutex;

int fd;

// Identifies whether or not the mutex should be called in the thread function
int G_MUTEX = 0;


// This function is used to "slow" things down. It is computationally heavy.
int fib(int n){
    if (n == 0 || n == 1)
        return 1;
    return fib(n-1) + fib(n-2);
}

/* Our thread function. It writes two messages to a file, with a small wait 
*  time in between (thanks to a call to fib()).
*  If the mutex flag was set in main(), then the global mutex is locked in this
*  function before the first write() statement, and ulocked after the second
*  write() statement.
*/
void * run_thread(void * arg){
    int * threadNum = arg;
    
    // If mutex flag was set in main(), lock the global mutex
    if (G_MUTEX)
		pthread_mutex_lock(&mutex);
    
    char buf[30];
    sprintf(buf,"Thread %d first message\n",*threadNum);
    write(fd,buf,strlen(buf));
    
    // Slow things down with excessive computations. 
    fib(35);
    sprintf(buf,"Thread %d second message\n",*threadNum);
    write(fd,buf,strlen(buf));
    
    // Unlock mutex
    if (G_MUTEX)
    	pthread_mutex_unlock(&mutex);
    return (void *)0;
}


int main(int argc, char * argv[]){
	
    // Set mutex flag is proper command line argument is given.
    if (argc == 2 && !strcmp(argv[1], "use_mutex"))
		G_MUTEX = 1;
	
    // Array of pthread IDs (used internally when creating and joining threads)
    pthread_t thread[NUM_THREADS];
	
    // Array of ints for indentifying thread order
    int id[NUM_THREADS];
    
    // Open file for writing
    fd = open("thread_file",O_RDWR | O_CREAT, S_IRWXU);
    
    // Initialize our global mutex variable
    pthread_mutex_init(&mutex,NULL);
    
    int i;
    // Create NUM_THREADS threads, passing an integer value between 0 and 
    // NUM_THREADS - 1 to each thread
    for (i = 0; i < NUM_THREADS; i++){
        id[i] = i;
        printf("Creating thread %d\n",i);
        pthread_create(&thread[i], NULL, run_thread, (void *) &id[i]);
    }

    // Block until all threads have exited
    for (i = 0; i < NUM_THREADS; i++){
        pthread_join(thread[i],NULL);
        printf("Joined thread %d\n",i);
    }

    // Clean up mutex
    pthread_mutex_destroy(&mutex);
    close(fd);
    return 0;
    
}
