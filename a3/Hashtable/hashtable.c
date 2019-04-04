#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hashtable.h"

void initHashtable(hashtable ** table, int Tsize){
    (*table) = malloc(sizeof(hashtable));
    (*table)->size = Tsize;
    (*table)->currentSize = 0;
    (*table)->hasharray = malloc(size*sizeof(Dataitem));
    for(int i = 0; i < size; i++) {
        (*table)->hasharray[i] = NULL;
    }
}

int hashcode(int key, hashtable *table) {
    return key % (table->size);
}

bool search(int key, hashtable *table, int time) {
   //get the hash 
   int hashIndex = hashcode(key, table);  
    
   //move in array until an empty 
   while(table->hasharray[hashIndex] != NULL) {
    
      if(table->hasharray[hashIndex]->key == key){
        if(time != 0){
            table->hasharray[hashIndex]->timeStamp = time;
        }
        return true; 
      }
            
      //go to next cell
      ++hashIndex;
        
      //wrap around the table
      hashIndex %= SIZE;
   }         
   return false;        
}

void insert(int key,int data, int time, hashtable *table) {

    struct Dataitem *item = (struct DataItem*)malloc(sizeof(struct DataItem));
    item->data = data;  
    item->key = key;
    item->timeStamp = time;

    table->currentSize++;
    if(table->currentSize > table->size) {
       deleteFirst(table);
    }

    //get the hash 
    int hashIndex = hashcode(key,table);

    //move in array until an empty or deleted cell
    while(table->hasharray[hashIndex] != NULL && table->hashArray[hashIndex]->key != -1) {
      //go to next cell
      ++hashIndex;
        
      //wrap around the table
      hashIndex %= SIZE;
    }
    
    table->hasharray[hashIndex] = item;
}

void deleteFirst(hashtable *table) {
    struct Dataitem *min = table->hasharray[0];
    for(int i = 1; i < table->size; i++){
        if(table->hasharray[i]->timeStamp < min->timeStamp){
            min = table->hasharray[i];
        }
    }
    min = NULL;
}