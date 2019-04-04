#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"

int main() {
    hashtable *t = NULL;
    initHashtable(&t, 5);
    for(int i = 0; i < 15; i++) {
        if(i < 10) {
            insert(10*i,10*i,i,t);
        }
        else {
            search((i-5)*10,t,0);
        }
        display(t);
    }
}