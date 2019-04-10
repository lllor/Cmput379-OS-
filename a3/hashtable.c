#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hashtable.h"

void initHashtable(hashtable ** table, int Tsize){
    (*table) = malloc(sizeof(hashtable));
    (*table)->size = Tsize;
    (*table)->hasharray = malloc(Tsize*sizeof(int **));
    for(int i = 0; i < Tsize; i++) {
        (*table)->hasharray[i] = malloc(sizeof(int *));
        (*table)->hasharray[i] = NULL;
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
    if(table->hasharray[hashIndex] != NULL) {
        if ((sizeof(table->hasharray[hashIndex])/sizeof(int)) == 1 && table->hasharray[hashIndex][0] == key){
            return true;
        }
        else{
           int size = (sizeof(table->hasharray[hashIndex])/sizeof(int));
           for(int i = 0; i<size;i++){
                if(table->hasharray[hashIndex][i] == key){
                    return true;
                }
            }
        }
    }         

   return false;        
}

void insert(int key, hashtable *table) {
    //get the hash 
    int hashIndex = hashcode(key,table);
    int* new;
    //move in array until an empty or deleted cell
    if(table->hasharray[hashIndex] != NULL) {
        new = realloc(table->hasharray[hashIndex],sizeof(int));
        new[(sizeof(new) / sizeof(int))-1] = key;
    }
    else{
        table->hasharray[hashIndex] = malloc(sizeof(int));
        table->hasharray[hashIndex][0] = key;
    }
    
}

void deleteItem(int key, hashtable *table) {

   //get the hash 
   int hashIndex = hashcode(key, table);

   //move in array until an empty
   if(table->hasharray[hashIndex] != NULL) {
        int size = (sizeof(table->hasharray[hashIndex])/sizeof(int));
        int *new_data = malloc((size-1)*sizeof(int));
        int j = 0;
        for(int i = 0; i<size;i++){
            if(table->hasharray[hashIndex][i] != key){
                new_data[j++] = table->hasharray[hashIndex][i];
            }
        }

        free(table->hasharray[hashIndex]);
        table->hasharray[hashIndex] = new_data;
   }            
}

void deleteAll(hashtable *table) {
  for(int i = 0; i < table->size; i++) {
      free(table->hasharray[i]);
      table->hasharray[i] = NULL;
  }
}

void display(hashtable *table) {
  for(int i = 0; i < table->size; i++) {
      if(table->hasharray[i] != NULL) {
          printf("%d | ",table->hasharray[i][0]);
      }
  }
  printf("\n");
}