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

void initHashtable(hashtable ** table, int Tsize);
int hashcode(int key, hashtable *table);
bool search(int key, hashtable *table, int time);
void insert(int key,int data, int time, hashtable *table);
void deleteFirst(hashtable *table);
void display(hashtable *table);