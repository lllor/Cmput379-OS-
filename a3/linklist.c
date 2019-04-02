#include "linklist.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void initLinklist(Linklist ** linklist)
{
	(*linklist)= malloc(sizeof(Linklist));
	(*linklist)->first = NULL;
	(*linklist)->last = NULL;
}
bool isEmpty(const Linklist *linklist){
    if(linklist->first == NULL && linklist->last == NULL){
        return true;
    }
    return false;
}
void addBack(Linklist *linklist, int pagenum){
    if (isEmpty(linklist)){
        struct  Node *newNode;
        newNode = malloc(sizeof(struct Node));
        newNode->pagenum = pagenum;
        newNode->next = NULL;

        linklist->first = newNode;
        linklist->last = newNode;
    }
    else{
        struct  Node *newNode;
        newNode = malloc(sizeof(struct Node));
        newNode->pagenum = pagenum;
        newNode->next = NULL;
        
        linklist->last->next = newNode;
        linklist->last = newNode;
    }
    
}
void leaveFront(Linklist *linklist){
    if(linklist->first == linklist->last){
        if(linklist->first!=NULL){
            free(linklist->first);
        }  
        linklist->first=NULL;
        linklist->last = NULL;
    }
    else{
        struct Node *temp = linklist->first->next;
        free(linklist->first);
        linklist->first = temp;
        return;
    }
}
void removeCurrent(Linklist *linklist, int pagenum)
{
    struct Node *pp = linklist->first;
    struct Node *temp = linklist->first;
    if (pagenum == linklist->first->pagenum){
        leaveFront(linklist);
        return;
    }
    while(pp!=NULL){
        temp = pp->next;
        if(temp == NULL){
            return;
        }
        //printf("%d - %d\n",temp->pagenum,pagenum);
        if(temp->pagenum == pagenum){
            pp->next = temp->next;
            free(temp);
            return;
        }
        
        pp = pp->next;
    }
}
void nuke(Linklist *linklist)
{
    while((linklist)->first!=NULL){
        leaveFront(linklist);
    }
}
void destroy(Linklist **linklist)
{
    nuke(*linklist);
    free(*linklist);
}