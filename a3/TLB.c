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
    //? first create an array(size lager is better) => flag_array[10000]
    //create a linklist => TLB
    //then read the page number from stdin => addr
    //?flag = addr/100000 (integr)
    //?the find flag_array[flag], this should proint to a linklist(size smaller is better)   => flag_list
    //?if "addr" is already in flag_list,  
    //find the addr in flag_list, move it the the end of TLB
    //?if "addr" is not in flag_list, add it to flag_list, 
    //add it to the end of TLB
    //if flag_list is full/if the TLB is full, remove the first pagenum from the TLB and add the addr to the end
    Linklist *TLB = NULL;
    initLinklist(&TLB);

    char buf[100] = {0};
    char addr_str[9] = {0};
    unsigned int addr;
    int position = 0;
    int hit = 0;
    int miss = 0;
    int TLB_size = 0;
    int addr_count = 0;
    int result = 0;
    int offset = 4;

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
        if(addr_count == flushPeriod){
            nuke(TLB);
            addr_count = 0;
        }
        
        strncpy(addr_str,buf+3,8);
        sscanf(addr_str,"%x",&addr);
        addr = addr >> offset;
        
        struct Node *pp=TLB->first;              //begin with the first element
        struct Node *tep=TLB->first;
	while(pp!=NULL)                          //til the end
        {
	    
            if(pp->next == NULL){
	        break;
	    }
	    tep = pp->next;
	    if(pp->pagenum == addr){
                pp->next = tep->next;
                temp->next = NULL;
                addBack(TLB,addr);
                free(tep);
                result = 1;
                break;
            }
            pp=pp->next;
        }
        
        
        if(result != 0) {
            hit++;
//            removeCurrent(TLB, addr);
//            addBack(TLB,addr);
        }
        else {
            miss++;
	    TLB_size += 1;
            if (TLB_size > tlbsize){
                leaveFront(TLB);
                addBack(TLB,addr);
            }
            else{
                addBack(TLB,addr);
            }
        }

        memset(buf,0,100);
        result = 0;
    }
    printf("%d %d %d\n",addr_count, hit, miss);
    destroy(&TLB);
}


void dealFIFO(int pgsize,int tlbsize,int flushPeriod, int ignoreI){
    Linklist *TLB = NULL;
    initLinklist(&TLB);

    char buf[100] = {0};
    char addr_str[9] = {0};
    unsigned int addr;
    int position = 0;
    int hit = 0;
    int miss = 0;
    int TLB_size = 0;
    int addr_count = 0;
    int result = 0;
    int offset = 4;
    for(;offset <= 16; offset++) {
        if(pow(2.0,(double)offset) == pgsize) {
            break;
        }
    }
   // printf("%d\n",offset);
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
        if(addr_count == flushPeriod){
            nuke(TLB);
            addr_count = 0;
        }
        
        strncpy(addr_str,buf+3,8);
        sscanf(addr_str,"%x",&addr);
        addr = addr >> offset;
        
        struct Node *pp=TLB->first;              //begin with the first element
        while(pp!=NULL)                          //til the end
        {
            if(pp->pagenum == addr){
                result = 1;
                break;
            }
            pp=pp->next;
        }
        
        
        if(result != 0) {
            hit++;
        }
        else {
	    TLB_size += 1;
            miss++;
            if (TLB_size > tlbsize){
                leaveFront(TLB);
                addBack(TLB,addr);
            }
            else{
                addBack(TLB,addr);
            }
            
        }
        memset(buf,0,100);
        result = 0;
    }
    printf("%d %d %d\n",addr_count, hit, miss);
    destroy(&TLB);
}
