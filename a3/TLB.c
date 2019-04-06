#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linklist.h"
#include <math.h>


int findAddr(unsigned int address, int pageNum[], int length);
void updatePageNumFIFO(unsigned int address, int *pageNum);
void clearAll(int *pageNum,int length);
void updatePageNumLRC(unsigned int address, int *pageNum, int length, int hit);
void dealFIFO(int pgsize,int tlbsize,int flushPeriod, int ignoreI);
void dealLRU(int pgsize,int tlbsize,int flushPeriod, int ignoreI);

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

    if (policy == 0){
        dealFIFO(pgsize,tlbsize,flushPeriod,ignoreI);
    }
    else{
        dealLRU(pgsize,tlbsize,flushPeriod,ignoreI);
    }


}
void dealLRU(int pgsize,int tlbsize,int flushPeriod, int ignoreI){
    Linklist *TLB = NULL;
    initLinklist(&TLB);

    char buf[100] = {0};
    char addr_str[9] = {0};
    unsigned int addr1;
    unsigned int addr2;
    int position = 0;
    int hit = 0;
    int miss = 0;
    int TLB_size = 0;
    int addr_count = 0;
    int result1 = 0;
    int result2 = 0;
    char delim[] = ",";
    char offset = 4;

    for(;offset <= 16; offset++) {
        if(pow(2, offset) == pgsize) {
            break;
        }
    }
    printf("%d\n",offset);
    while(fgets(buf,sizeof(buf),stdin)) {
        if(buf[0] == '=') {
            continue;
        }
        if(ignoreI == 1) {
            if(buf[0] == 'I') {
                continue;
            }
        }
        addr_count += 1;
        
        char *ptr = strtok(buf,delim);
        int size = atoi(strtok(NULL,delim));
        
        strncpy(addr_str,buf+3,8);
        sscanf(addr_str,"%x",&addr1);
        addr2 = addr1+size;
        addr1 = addr1 >> offset;
        addr2 = addr2 >> offset;
        
        if (addr1 == addr2){
            struct Node *pp=TLB->first;              //begin with the first element
            while(pp!=NULL)                          //til the end
            {
                if(pp->pagenum == addr1){
                    result1 = 1;
                    result2 = 0;
                    break;
                }
                pp=pp->next;
            }
        }
        else{
            struct Node *pp=TLB->first;              //begin with the first element
            while(pp!=NULL)                          //til the end
            {
                if(pp->pagenum == addr1){
                    result1 = 1;
                    break;
                }
                if(pp->pagenum == addr2){
                    result2 = 1;
                    break;
                }
                pp=pp->next;
            }
        }
        
        
        if(result1 != 0) {
            hit++;
        }
        else {
            miss++;
            TLB_size += 1;
            if (TLB_size > tlbsize){
                //leaveFront(TLB);
                struct Node *temp = TLB -> first;
                temp = temp->next;
                TLB->first = temp;
                addBack(TLB,addr1);
            }
            else{
                addBack(TLB,addr1);
            }
        }

        if(result2 != 0){
            hit++;
        }
        else {
            miss++;
            TLB_size += 1;
            if (TLB_size > tlbsize){
                //leaveFront(TLB);
                struct Node *temp = TLB -> first;
                temp = temp->next;
                TLB->first = temp;
                addBack(TLB,addr2);
            }
            else{
                addBack(TLB,addr2);
            }
        }

        memset(buf,0,100);
        result1 = 0;
        result2 = 0;
        if(addr_count == flushPeriod){
            nuke(TLB);
            addr_count = 0;
        }
    }
    printf("%d %d %d\n",addr_count, hit, miss);
    destroy(&TLB);
}
void dealFIFO(int pgsize,int tlbsize,int flushPeriod, int ignoreI){
    Linklist *TLB = NULL;
    initLinklist(&TLB);

    char buf[100] = {0};
    char addr_str[9] = {0};
    unsigned int addr1;
    unsigned int addr2;
    int position = 0;
    int hit = 0;
    int miss = 0;
    int TLB_size = 0;
    int addr_count = 0;
    int result1 = 0;
    int result2 = 0;
    char delim[] = ",";
    char offset = 4;
    
    for(;offset <= 16; offset++) {
        if(pow(2, offset) == pgsize) {
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
        addr_count += 1;
        
        char *ptr = strtok(buf,delim);
        int size = atoi(strtok(NULL,delim));
        
        strncpy(addr_str,buf+3,8);
        sscanf(addr_str,"%x",&addr1);
        addr2 = addr1+size;
        addr1 = addr1 >> offset;
        addr2 = addr2 >> offset;
        
        if (addr1 == addr2){
            struct Node *pp=TLB->first;              //begin with the first element
            while(pp!=NULL)                          //til the end
            {
                if(pp->pagenum == addr1){
                    result1 = 1;
                    result2 = 0;
                    break;
                }
                pp=pp->next;
            }
        }
        else{
            addr_count += 1;
            struct Node *pp=TLB->first;              //begin with the first element
            while(pp!=NULL)                          //til the end
            {
                if(pp->pagenum == addr1){
                    result1 = 1;
                    if(result2 == 1){
                        break;
                    }
                    
                }
                if(pp->pagenum == addr2){
                    result2 = 1;
                    if(result1 == 1){
                        break;
                    }
                }
                pp=pp->next;
            }
        }
        
        
        
        if(result1 != 0) {
            hit++;
        }
        else {
            miss++;
            if (TLB_size >= tlbsize){
                leaveFront(TLB);
                addBack(TLB,addr1);
            }
            else{
                addBack(TLB,addr1);
            }
            
        }
        
        if(addr_count == flushPeriod){
            nuke(TLB);
            addr_count = 0;
        }

        if(result2 != 0) {
            hit++;
        }
        else {
            miss++;
            
            if (TLB_size >= tlbsize){
                leaveFront(TLB);
                addBack(TLB,addr2);
            }
            else{
                addBack(TLB,addr2);
            }
            
        }

        memset(buf,0,100);
        result1 = 0;
        result2 = 0;
        if(addr_count == flushPeriod){
            nuke(TLB);
            addr_count = 0;
        }
    }
    printf("%d %d\n",hit, miss);
    destroy(&TLB);
}
