#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linklist.h"
#include <math.h>

int dealFIFO(Linklist *TLB, unsigned int addr);
int dealLRU(Linklist *TLB, unsigned int addr);

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
    int result = 0;
    int offset = 4;
    char delim[] = ",";

    for(;offset <= 16; offset++) {
        if(pow(2, offset) == pgsize) {
            break;
        }
    }
    //printf("%d\n",offset);
    while(fgets(buf,sizeof(buf),stdin)) {
        if(buf[0] == '=') {
            continue;
        }
        if(ignoreI == 1) {
            if(buf[0] == 'I') {
                continue;
            }
        }

        char *ptr = strtok(buf,delim);
        int size = atoi(strtok(NULL,delim));
        addr_count += 1;
        if(addr_count == flushPeriod){
            nuke(TLB);
            addr_count = 0;
        }
        
        strncpy(addr_str,buf+3,8);
        sscanf(addr_str,"%x",&addr1);
        addr2 = addr1+size;
        addr1 = addr1 >> offset;
        addr2 = addr2 >> offset;
        
        if (policy == 0){
            result = dealFIFO(TLB,addr1);
        }
        else{
            result = dealLRU(TLB,addr1);
        }

        if(result != 0) {
            hit++;
        }
        else {
            miss++;
            TLB_size += 1;
            if (TLB_size > tlbsize){
                leaveFront(TLB);
                addBack(TLB,addr1);
            }
            else{
                addBack(TLB,addr1);
            }
        }

        if(addr1 != addr2) {
            addr_count++;
            if (policy == 0){
                result = dealFIFO(TLB,addr2);
            }
            else{
                result = dealLRU(TLB,addr2);
            }

            if(result != 0) {
                hit++;
            }
            else {
                miss++;
                TLB_size += 1;
                if (TLB_size > tlbsize){
                    leaveFront(TLB);
                    addBack(TLB,addr2);
                }
                else{
                    addBack(TLB,addr2);
                }
            }
        }

        memset(buf,0,100);
        result = 0;
    }
    printf("%d %d %d\n",addr_count, hit, miss);
    destroy(&TLB);
}


int dealLRU(Linklist *TLB, unsigned int addr){
    struct Node *pp=TLB->first;              //begin with the first element
    struct Node *tep=TLB->first;
    int result = 0;
    while(pp!=NULL)                          //til the end
    {
        if(pp->next == NULL){
            result = 0;
            break;
        }
        if(TLB->first->pagenum == addr){
            leaveFront(TLB);
            addBack(TLB,addr);
            result = 1;
            break;
        }
        if(TLB->last->pagenum == addr){
            result = 1;
            break;
        }
        tep = pp->next;
        if(tep->pagenum == addr){
            pp->next = tep->next;
            tep->next = NULL;
            addBack(TLB,addr);
            free(tep);
            result = 1;
            break;
        }
        pp=pp->next;
    }
    return result;
}


int dealFIFO(Linklist *TLB, unsigned int addr){
    int result = 0;
    struct Node *pp=TLB->first;              //begin with the first element
    while(pp!=NULL)                          //til the end
    {
        if(pp->pagenum == addr){
            result = 1;
            break;
        }
        pp=pp->next;
    }
    return result;
}
