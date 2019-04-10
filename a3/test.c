#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "linklist.h"

int main() {
    Linklist *TLB = NULL;
    initLinklist(&TLB);
    for(int i = 0; i < 10; i++){
        addBack(TLB,i*10);
    }
    displayLink(TLB);
    removeCurrent(TLB,20);
    displayLink(TLB);
    removeCurrent(TLB,90);
    displayLink(TLB);
    removeCurrent(TLB,80);
    displayLink(TLB);
    removeCurrent(TLB,0);
    displayLink(TLB);
    addBack(TLB,90);
    displayLink(TLB);
}
