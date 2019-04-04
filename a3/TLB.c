#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hashtable.h"

int main(int argc, char *argv[]) {
    int ignoreI = 0;
    int flushPeriod = 0;
    int policy = -1;
    int pgsize = 0;
    int tlbsize = 0;

    
    if(argc < 5 || argc > 8) {
        exit(1);
    }

    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i],"-i") == 0) {
            ignoreI = 1;
        }
        else if(strcmp(argv[i],"-f") == 0) {
            flushPeriod = atoi(argv[++i]);
        }
        else if(strcmp(argv[i],"-p") == 0) {
            if(strcmp(argv[i+1], "FIFO") == 0) {
                policy = 0;
            }
            else if(strcmp(argv[i+1], "LRU") == 0) {
                policy = 1;
            }
        }
        else if(i == argc-2) {
            pgsize = atoi(argv[i]);
        }
        else if(i == argc-1) {
            tlbsize = atoi(argv[i]);
        }
    } 

    if(policy == -1 || pgsize == 0 || tlbsize == 0) {
        exit(1);
    }



    char buf[100] = {0};
    char addr_str[9] = {0};
    unsigned int addr;
    int position = 0;
    int hit = 0;
    int miss = 0;
    int offset = 4;

    for(;offset <= 16; offset++) {
        if((int)pow(2,offset) == pgsize){
            break;
        }
    }

    while(fgets(buf,sizeof(buf),stdin)) {
        if(buf[0] == '=') {
            continue;
        }
        if(ignoreI == 1) {
            if(buf[0] == 'I') {
                continue;
            }
        }
        //printf("%d\n",buf[1]);
        strncpy(addr_str,buf+3,8);
        sscanf(addr_str,"%x",&addr);
        addr = addr >> offset;
        
        memset(buf,0,100);
    }
    printf("%d %d\n",hit, miss);
    free(pageNumber);
}