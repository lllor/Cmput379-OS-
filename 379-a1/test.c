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
#include <errno.h>

void main(){
    int integer;
    int result = scanf("%d",&integer);
    if(result>0){
        printf("get input\n");
        
    }
    else{
        printf("not get\n");
        
    }
    
}