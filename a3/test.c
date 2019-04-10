#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"

int main() {
    hashtable *t = NULL;
    initHashtable(&t, 5);
    for(int i = 0; i < 11; i++) {
        if(i < 5) {
            insert(10*i,10*i,i,t);
        }
        else {
            printf("%d\n",search((i-5)*10,t));
        }
        display(t);
    }
    free(t);
}