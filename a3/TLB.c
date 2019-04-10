#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linklist.h"
#include <math.h>
#include "hashtable.h"

int dealLRU2addr(Linklist *TLB, unsigned int addr1, unsigned int addr2, int maxSize);
int dealLRU1addr(Linklist *TLB, unsigned int addr1, int maxSize);
int dealFIFO1addr(Linklist *TLB, hashtable *t, unsigned int addr, int maxSize);
int dealFIFO2addr(Linklist *TLB, hashtable *t, unsigned int addr1, unsigned int addr2, int maxSize);

int main(int argc, char *argv[]) {
    int ignoreI = 0;
    int flushPeriod = 0;
    int policy = -1;
    int pgsize = 0;
    int tlbsize = 0;
    FILE *fp;
    fp = fopen("table.csv","w");
    fprintf(fp,"%s,%s,%s\n","total","hit","miss");
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
    hashtable *table = NULL;
    initHashtable(&table, tlbsize);

    char buf[100] = {0};
    char addr_str[9] = {0};
    unsigned int addr1;
    unsigned int addr2;
    long long int hit = 0;
    long long int miss = 0;
    long long int  addr_count = 0;
    long long int  result = 0;
    int offset = 4;
    char delim[] = ",";
    long long int total_reference = 0;

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
        total_reference++;
        int flag;
        if(addr_count == flushPeriod){
            flag = 1;
        }
        else{
            flag = 0;
        }
        strncpy(addr_str,buf+3,8);
        sscanf(addr_str,"%x",&addr1);
        addr2 = addr1+size;
        addr1 = addr1 >> offset;
        addr2 = addr2 >> offset;
        
        if(policy == 0) {
            if(addr1 == addr2) {
                result = dealFIFO1addr(TLB, table, addr1, tlbsize);
            }
            else {
                result = dealFIFO2addr(TLB, table, addr1, addr2, tlbsize);
                addr_count++;
                total_reference++;
            }
        }
        else {
            if(addr1 == addr2) {
                result = dealLRU1addr(TLB, addr1, tlbsize);
            }
            else {
                result = dealLRU2addr(TLB, addr1, addr2, tlbsize);
                addr_count++;
                total_reference++;
            }
        }

        if(result == 0){
            miss += 2;
        }
        else if(result == 1) {
            hit++;
            miss++;
        }
        else if(result == 2) {
            hit += 1 - flag;
            miss += 1 + flag;
        }
        else if(result == 3) {
            hit += 2-flag;
            miss += flag;
        }
        else if(result == 4) {
            miss++;
        }
        else if(result == 5) {
            hit++;
        }

        if(flushPeriod != 0 && addr_count >= flushPeriod) {
            nuke(TLB);
            deleteAll(table);
            addr_count -= flushPeriod;
            if(result == 0 || result == 1 || result == 2 || result == 3){
                addBack(TLB, addr2);
                insert(addr2, table);
            }
        }
        //printf("%d %d %d\n", total_reference , hit, miss);
        memset(buf,0,100);
        result = 0;
    }
    printf("%llu\n%llu\n%llu\n", total_reference , hit, miss);
    fclose(fp);
    destroy(&TLB);
    destroy_hash(&table);
}


int dealLRU2addr(Linklist *TLB, unsigned int addr1, unsigned int addr2, int maxSize) {
    struct Node *pp=TLB->first;              //begin with the first element
    struct Node *tep=TLB->first;
    int result = 0;

    if(isEmpty(TLB)){
        addBack(TLB,addr1);
        addBack(TLB,addr2);
        return result;
    }

    if(TLB->first->pagenum == addr1){
        leaveFront(TLB);
        addBack(TLB,addr1);
        result += 1;
    }
    else if(TLB->last->pagenum == addr1){
        result += 1;
    }

    if(TLB->first->pagenum == addr2){
        leaveFront(TLB);
        addBack(TLB,addr2);
        result += 2;
    }
    else if(TLB->last->pagenum == addr2){
        result += 2;
    }

    while(pp!=NULL)                          //til the end
    {
        if(pp->next == NULL || result == 3){
            break;
        }

        tep = pp->next;
        if(tep->pagenum == addr1 && result != 1){
            pp->next = tep->next;
            tep->next = NULL;
            addBack(TLB,addr1);
            free(tep);
            result += 1;
        }
        else if(tep->pagenum == addr2 && result != 2) {
            pp->next = tep->next;
            tep->next = NULL;
            addBack(TLB,addr2);
            free(tep);
            result += 2;
        }
        pp=pp->next;
    }

    if(result == 0) {
        addBack(TLB,addr1);
        addBack(TLB,addr2);
    }
    else if(result == 1){
        addBack(TLB,addr2);
    }
    else if(result == 2){
        addBack(TLB,addr1);
    }

    while(TLB->size > maxSize) {
        leaveFront(TLB);
    }

    return result;
}

int dealLRU1addr(Linklist *TLB, unsigned int addr1, int maxSize) {
    struct Node *pp=TLB->first;              //begin with the first element
    struct Node *tep=TLB->first;
    int result = 4;

    if(isEmpty(TLB)){
        addBack(TLB,addr1);
        return result;
    }

    if(TLB->first->pagenum == addr1){
        leaveFront(TLB);
        addBack(TLB,addr1);
        result += 1;
        return result;
    }
    else if(TLB->last->pagenum == addr1){
        result += 1;
        return result;
    }

    while(pp!=NULL)                          //til the end
    {
        if(pp->next == NULL || result == 5){
            break;
        }

        tep = pp->next;
        if(tep->pagenum == addr1){
            pp->next = tep->next;
            tep->next = NULL;
            addBack(TLB,addr1);
            free(tep);
            result += 1;
            break;
        }
        pp=pp->next;
    }

    if(result == 4){
        addBack(TLB,addr1);
    }

    while(TLB->size > maxSize) {
        leaveFront(TLB);
    }

    return result;
}


int dealFIFO1addr(Linklist *TLB, hashtable *t, unsigned int addr, int maxSize){
    int result = 4;
    if(!search(addr, t)) {
        if(TLB->size == maxSize) {
            int temp = leaveFront(TLB);
            deleteItem(temp, t);
            addBack(TLB, addr);
            insert(addr,t);
        }
        else {
            addBack(TLB,addr);
            insert(addr,t);
        }
    }
    else {
        result += 1;
    }
    return result;
}

int dealFIFO2addr(Linklist *TLB, hashtable *t, unsigned int addr1, unsigned int addr2, int maxSize){
    int result = 0;
    if(!search(addr1, t)) {
        if(TLB->size >= maxSize) {
            int temp = leaveFront(TLB);
            deleteItem(temp, t);
            addBack(TLB, addr1);
            insert(addr1,t);
        }
        else {
            addBack(TLB,addr1);
            insert(addr1,t);
        }
    }
    else {
        result += 1;
    }

    if(!search(addr2, t)) {
        if(TLB->size >= maxSize) {
            int temp = leaveFront(TLB);
            deleteItem(temp, t);
            addBack(TLB, addr2);
            insert(addr2,t);
        }
        else {
            addBack(TLB,addr2);
            insert(addr2,t);
        }
    }
    else {
        result += 2;
    }

    return result;
}