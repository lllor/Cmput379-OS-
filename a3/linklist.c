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
    (*linklist)->size = 0;
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
    linklist->size++;
}

int leaveFront(Linklist *linklist){
    int page;
    if(linklist->first == linklist->last){
        if(linklist->first!=NULL){
            page = linklist->first->pagenum;
            free(linklist->first);
            linklist->first=NULL;
            linklist->last = NULL;
            linklist->size--;
        }  
        else {
            page = -1;
        }

    }
    else{
        struct Node *temp = linklist->first->next;
        page = linklist->first->pagenum;
        free(linklist->first);
        linklist->first = temp;
        linklist->size--;
    }
    return page;
}

void removeCurrent(Linklist *linklist, int pagenum)
{
    struct Node *pp = linklist->first;
    struct Node *temp = linklist->first;
    if(isEmpty(linklist)) {
        return;
    }
    else if(pagenum == linklist->first->pagenum){
        leaveFront(linklist);
        return;
    }

    while(pp!=NULL){
        temp = pp->next;
        if(temp == NULL){
            break;
        }
        //printf("%d\n",temp->pagenum);
        //printf("%d - %d\n",temp->pagenum,pagenum);
        if(temp->pagenum == pagenum){
            if(temp->next == NULL) {
                linklist->last = pp;
            }
            pp->next = temp->next;
            temp->next = NULL;
            free(temp);
            break;
        }
        
        pp = pp->next;
    }
    linklist->size--;
}

void displayLink(Linklist *linklist) {
    struct Node *temp = linklist->first;
    while(temp != NULL) {
        printf("%d ",temp->pagenum);
        temp = temp->next;
    }
    printf("\n");
}

void nuke(Linklist *linklist)
{
    while((linklist)->first!=NULL){
        leaveFront(linklist);
    }
    linklist->size = 0;
}
void destroy(Linklist **linklist)
{
    nuke(*linklist);
    free(*linklist);
}