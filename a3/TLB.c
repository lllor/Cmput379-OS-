#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int findAddr(unsigned int address, int pageNum[], int length);
void updatePageNumFIFO(unsigned int address, int *pageNum);

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
            else if(strcmp(argv[i+1], "LRC") == 0) {
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

    int pageNumber[tlbsize];
    for(int i = 0; i < tlbsize; i++) {
        pageNumber[i] = -1;
    }

    char buf[100] = {0};
    char addr_str[9] = {0};
    unsigned int addr;
    int position = 0;
    int hit = 0;
    int miss = 0;

    while(fgets(buf,sizeof(buf),stdin)) {
        if(buf[0] == '=') {
            continue;
        }
        strncpy(addr_str,buf+3,8);
        sscanf(addr_str,"%x",&addr);
        addr = addr >> pgsize;
        if(findAddr(addr,pageNumber,tlbsize)) {
            hit++;
        }
        else {
            miss++;
            if(policy == 0) {
                updatePageNumFIFO(addr, &pageNumber[position]); 
                position = (position+1) % tlbsize;
            }
        }
        if(policy == 1) {
            //updatePageNumLRC();
        }
        memset(buf,0,100);
    }
}

int findAddr(unsigned int address, int pageNum[], int length) {
    for(int i = 0; i < length; i++) {
        if(pageNum[i] == address){
            return 1;
        }
    }
    return 0;
}

void updatePageNumFIFO(unsigned int address, int *pageNum) {
    *pageNum = address;
}

// int updatePageNumLRC() {

// }
