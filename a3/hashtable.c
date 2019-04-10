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
        //(*table)->hasharray[i] = malloc(sizeof(int *));
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
        int size = table->hasharray[hashIndex][0];
        //printf("%d\n",size);
        if (table->hasharray[hashIndex][1] == key && size == 1){
            return true;
        }
        else{
            for(int i = 1; i<=size;i++){
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
    //move in array until an empty or deleted cell
    if(table->hasharray[hashIndex] != NULL && table->hasharray[hashIndex][0] != 0) {
       // int size = table->hasharray[hashIndex][0];
       // printf("%d\n",size);
        int size = table->hasharray[hashIndex][0];
        table->hasharray[hashIndex] = realloc(table->hasharray[hashIndex],sizeof(int)*(2+size));
        table->hasharray[hashIndex][size+1] = key;
        table->hasharray[hashIndex][0] = size+1;

    }
    else{
        table->hasharray[hashIndex] = malloc(sizeof(int)*2);
        table->hasharray[hashIndex][0] = 1;
        table->hasharray[hashIndex][1] = key;
    }
    
}

void deleteItem(int key, hashtable *table) {

    //get the hash 
    int hashIndex = hashcode(key, table);

    //move in array until an empty
    if(table->hasharray[hashIndex] != NULL) {
        int size = table->hasharray[hashIndex][0];
        
        int j = 1;
        
        if(size>1){
            int *new_data = malloc((size)*sizeof(int));
            new_data[0] = size;
            for(int i = 1; i<=size;i++){
              if(table->hasharray[hashIndex][i] != key){
                  new_data[j] = table->hasharray[hashIndex][i];
                  j+=1;
              }
            }
            free(table->hasharray[hashIndex]);
            table->hasharray[hashIndex] = new_data;
        }
        else{
            free(table->hasharray[hashIndex]);
            table->hasharray[hashIndex] = NULL;
        }
   }            
}

void deleteAll(hashtable *table) {
  for(int i = 0; i < table->size; i++) {
      free(table->hasharray[i]);
      table->hasharray[i] = NULL;
  }
}
void destroy_hash(hashtable ** table){
    for(int i = 0; i < (*table)->size; i++) {
      free((*table)->hasharray[i]);
  }
    free((*table)->hasharray);
    free(*table);
    //free(table);
}
void display(hashtable *table) {
  for(int i = 0; i < table->size; i++) {
      if(table->hasharray[i] != NULL) {
          printf("%d | ",table->hasharray[i][0]);
      }
  }
  printf("\n");
}