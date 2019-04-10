#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hashtable.h"

void initHashtable(hashtable ** table, int Tsize){
    (*table) = malloc(sizeof(hashtable));
    (*table)->size = Tsize;
    (*table)->hasharray = malloc(Tsize*sizeof(int));
    for(int i = 0; i < Tsize; i++) {
        (*table)->hasharray[i] = -1;
    }
}

int hashcode(int key, hashtable *table) {
    return key % (table->size);
}

bool search(int key, hashtable *table) {
    //get the hash 
    int hashIndex = hashcode(key, table);  
    int inithashIndex = hashIndex;
    //move in array until an empty 
    while(table->hasharray[hashIndex] != -1) {
      if(table->hasharray[hashIndex] == key){
        return true; 
      }
            
      //go to next cell
      ++hashIndex;

      //wrap around the table
      hashIndex %= table->size;
      if(hashIndex == inithashIndex) {
        return false;
      }
   }         
   return false;        
}

void insert(int key, hashtable *table) {
    //get the hash 
    int hashIndex = hashcode(key,table);

    //move in array until an empty or deleted cell
    while(table->hasharray[hashIndex] != -1) {
      //go to next cell
      ++hashIndex;
        
      //wrap around the table
      hashIndex %= table->size;
    }
    
    table->hasharray[hashIndex] = key;
}

void deleteItem(int key, hashtable *table) {

   //get the hash 
   int hashIndex = hashcode(key, table);

   //move in array until an empty
   while(table->hasharray[hashIndex] != -1) {
      if(table->hasharray[hashIndex] == key) {
         table->hasharray[hashIndex] = -1;
      }
    
      //go to next cell
      ++hashIndex;
    
      //wrap around the table
      hashIndex %= table->size;
   }            
}

void deleteAll(hashtable *table) {
  for(int i = 0; i < table->size; i++) {
    table->hasharray[i] = -1;
  }
}

void display(hashtable *table) {
  for(int i = 0; i < table->size; i++) {
    if(table->hasharray[i] != -1) {
      printf("%d | ",table->hasharray[i]);
    }
  }
  printf("\n");
}