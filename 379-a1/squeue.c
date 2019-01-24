#include "squeue.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/*CMPUT 201 Student Submission License Version 1.0
Copyright 2018 <student name>
Unauthorized redistribution is forbidden under all circumstances. Use of this software without explicit authorization from the author and the CMPUT 201 Instructor is prohibited.
This software was produced as a solution for an assignment or lab in the course CMPUT 201 - Practical Programming Methodology at the University of Alberta, Canada. This solution is confidential and remains confidential after it is submitted for grading. The course staff has the right to run plagiarism-detection tools on any code developed under this license, even beyond the duration of the course.
Copying any part of this solution without including this copyright notice is illegal.
If any portion of this software is included in a solution submitted for grading at an educational institution, the submitter will be subject to the plagiarism sanctions at that institution.
This software cannot be publicly posted under any circumstances, whether by the original student or by a third party. If this software is found in any public website or public repository, the person finding it is kindly requested to immediately report, including the URL or other repository locating information, to the following email address:
      nadi@ualberta.ca
*/
void initSqueue (Squeue ** squeue)
{
	(*squeue)=malloc(sizeof(Squeue));				//malloc eough space for a squeue
	(*squeue)->first=NULL;						//this is a empty squeue 
	(*squeue)->last =NULL;
}
bool isEmpty (const Squeue *squeue)
{
	if(squeue->first==NULL && squeue->last==NULL)			//if the first element and last element are both empty, then this is a empty squeue
		return true;
	return false;
}
void addFront (Squeue *squeue, char *val)
{
	if(isEmpty(squeue))						//if this is a empty squeue
	{
		struct Node *newNode;
		newNode = malloc(sizeof(struct Node));			//we malloc a space for the node
		char *a=malloc(strlen(val)+1);				//we also malloc a space for the val
		strcpy(a,val);						//get the val
		newNode->val=a;						//assigned the val to the node
		newNode->prev=NULL;					//there is only one node, so prev==NULL
		newNode->next=squeue->first;				//the next it NULL
		squeue->first=newNode;					//now, the first/last element in the squeue is same
		squeue->last=newNode;
	}       
	else
	{
		struct Node *newNode;					//if this is not an empty squeue
		newNode = malloc(sizeof(struct Node));			
		char *a=malloc(strlen(val)+1);
		strcpy(a,val);
		newNode->val=a;						
		newNode->prev=NULL;					//this is a addFront function, will never have a front node
		newNode->next=squeue->first;				//set this node as the first element
		squeue->first->prev=newNode;				//set other nodes after it
		squeue->first=newNode;
	}       
} 
void addBack (Squeue *squeue, char val)
{
	if(isEmpty(squeue))						//if this is a empty squeue
	{
		struct Node *newNode;
		newNode = malloc(sizeof(struct Node));
		char a=malloc(strlen(val)+1);
		strcpy(a,val);
		newNode->val=a;
		newNode->next=NULL;
		newNode->prev=squeue->last;
		squeue->last=newNode;
		squeue->first=newNode;					//now, the first/last element in the squeue is same
	}
	else
	{								//if this is not an empty squeue
		struct Node *newNode;
		newNode = malloc(sizeof(struct Node));
		char a=malloc(strlen(val)+1);
		strcpy(a,val);
		newNode->val=a;
		newNode->next=NULL;					//this is a addBack function, will never have a next node
		newNode->prev=squeue->last;				//set this node as the last element
		squeue->last->next=newNode;				//set other nodes before it
		squeue->last=newNode;
	}
}
char peekFront (const Squeue *squeue)	
{
	assert (!isEmpty(squeue));
	return ((squeue->first)->val);					//to get the val of the first element
}	
char* peekBack (const Squeue *squeue)
{
	assert (!isEmpty(squeue));
	return ((squeue->last)->val);					//to get the val of the last element
}      
void mergeFront(Squeue *squeue)
{
	assert(squeue->first!=squeue->last);				//make sure there is at least element in the queue
	char *a=squeue->first->val;
	char *b=squeue->first->next->val;
	char *c=(char *)malloc(1+strlen(a)+strlen(b));			//assigned enough space to cat two string
	strcpy(c,a);
	strcat(c,b);
	leaveFront(squeue);						//delet the first two node
        leaveFront(squeue);
        addFront(squeue,c);						//add c as the first node
        free(c);							//free c
//	squeue->first->val=c;
}
void print (const Squeue *squeue, char direction)
{
	
	if (direction=='f')						//print in order
	{
		printf("stack is:\n");
		struct Node *pp=squeue->first;				//begin with the first element
		while(pp!=NULL)						//til the end
		{
			printf("\t%s\n",pp->val);
			pp=pp->next;
		}
	}
	else if(direction=='r')						//print in reverse
	{
		printf("stack is:\n");
		struct Node *pp=squeue->last;				//begin with the last
		while(pp!=NULL)
		{
			printf("\t%s\n",pp->val);			
			pp=pp->prev;
		}
	}
	else
		fprintf(stderr,"Error, illegal direction <%c>",direction);
}
void reverse(Squeue *squeue)
{
	Squeue *new=malloc(sizeof(Squeue));				//build a new squeue
	new->first=NULL;
	new->last=NULL;
	while((squeue)->first!=NULL)					//store all node from squeue, peekfront but add to the end
	{
		addBack(new,peekFront(squeue));
		leaveFront(squeue);					//empty the original squeue
	}
	while((new)->first!=NULL)					//store all node form temp squeue into orignal squeue
	{
		addFront(squeue,peekFront(new));
		leaveFront(new);
	}
	free(new);							//free
}
void leaveFront (Squeue *squeue)		
{
	assert(!isEmpty(squeue));					//make sure this is not an empty squeue
	if(squeue->first==squeue->last)					//if there is only one nodes
	{
		free(squeue->first->val);				//free the memory
		free(squeue->first);
		squeue->first=NULL;					//set the squeue as empty
		squeue->last=NULL;
	}
	else
	{
		struct Node *temp=squeue->first->next;			//else bulid a temp to store the first->next
		free(squeue->first->val);				//free the memory
		free(squeue->first);
		squeue->first=temp;					//set the first->next as first
		squeue->first->prev=NULL;
	}
}
void mergeBack(Squeue *squeue)
{       
	assert(squeue->first!=squeue->last);				//make sure there is at least two element in the queue
	char *a=squeue->last->val;
	char *b=squeue->last->prev->val;
	char *c=(char *)malloc(1+strlen(a)+strlen(b));			//assigned enough space to cat two string
	strcpy(c,b);
	strcat(c,a);
	leaveBack(squeue);						//delet the last two node
	leaveBack(squeue);
	addBack(squeue,c);						//add c as the last node
	free(c);
}
void leaveBack (Squeue *squeue)
{
	assert(!isEmpty(squeue));					//make sure this is not an empty squeue
	if(squeue->first==squeue->last)					//if there is only one nodes
	{
		free(squeue->last->val);
		free(squeue->last);					//free the memory
		squeue->first=NULL;
		squeue->last=NULL;					//set the squeue as empty 
	}
	else
	{
		struct Node *temp=squeue->last->prev;			//else bulid a temp to store the last->prev
		free(squeue->last->val);
		free(squeue->last);					//free the memory
		squeue->last=temp;
		squeue->last->next=NULL;				//set the last->prev as last
//              free(squeue->last->next);
	}

}	
void nuke (Squeue *squeue)
{
	while((squeue)->first!=NULL)					//free the memort til the squeue is empty
	{
		leaveFront(squeue);
	}
}
void destroySqueue(Squeue **squeue)
{
        nuke(*squeue);							//free memory
	free(*squeue);							//free squeue itself
//      squeue=NULL;
}
