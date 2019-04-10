#include <stdbool.h>

typedef struct {
    int size;
    int **hasharray;
}hashtable;

void initHashtable(hashtable ** table, int Tsize);
int hashcode(int key, hashtable *table);
bool search(int key, hashtable *table);
void insert(int key, hashtable *table);
void deleteItem(int key, hashtable *table);
void deleteAll(hashtable *table);
void display(hashtable *table);