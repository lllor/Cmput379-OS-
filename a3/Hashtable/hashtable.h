#include <stdbool.h>

struct Dataitem {
    int key;
    int Data;
    int timeStamp;
};

typedef struct {
    int size;
    int currentSize;
    struct Dataitem *hasharray;
}hashtable;