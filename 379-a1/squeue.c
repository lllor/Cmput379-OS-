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

void addBack (Squeue *squeue, char val)
{
	if(isEmpty(squeue))						//if this is a empty squeue
	{
		struct Node *newNode;
		newNode = malloc(sizeof(struct Node));
		//char a=malloc(1+1);
		//strcpy(a,val);
		char a = val;
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
		//char *a=malloc(1+1);
		char a = val;
		//strcpy(a,val);
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
char peekBack (const Squeue *squeue)
{
	assert (!isEmpty(squeue));
	return ((squeue->last)->val);					//to get the val of the last element
}
void nuke (Squeue *squeue)
{
	while((squeue)->first!=NULL)					//free the memort til the squeue is empty
	{
		leaveFront(squeue);
	}
}
void leaveFront (Squeue *squeue)
{
	assert(!isEmpty(squeue));					//make sure this is not an empty squeue
	if(squeue->first==squeue->last)					//if there is only one nodes
	{
		//free(squeue->first->val);				//free the memory
		//free(squeue->first);
		squeue->first=NULL;					//set the squeue as empty
		squeue->last=NULL;
	}
	else
	{
		struct Node *temp=squeue->first->next;			//else bulid a temp to store the first->next
		//free(squeue->first->val);				//free the memory
		//free(squeue->first);
		squeue->first=temp;					//set the first->next as first
		squeue->first->prev=NULL;
	}
}

void leaveBack(Squeue *squeue)
{
	assert(!isEmpty(squeue));					//make sure this is not an empty squeue
	if(squeue->first==squeue->last)					//if there is only one nodes
	{
		//free(squeue->first->val);				//free the memory
		//free(squeue->first);
		squeue->first=NULL;					//set the squeue as empty
		squeue->last=NULL;
	}
	else
	{
		struct Node *temp=squeue->last->prev;			//else bulid a temp to store the first->next
		//free(squeue->first->val);				//free the memory
		//free(squeue->first);
		squeue->last=temp;					//set the first->next as first
		squeue->last->next=NULL;
	}
}

void print (const Squeue *squeue, char direction, char iferror)
{
	//assert(!isEmpty(squeue));
	if (direction=='f')						//print in order
	{
		printf("stack is:\n");
		struct Node *pp=squeue->first;				//begin with the first element
		printf("%c ", iferror);
		while(pp!=NULL)						//til the end
		{

			printf("%c",pp->val);
			pp=pp->next;
		}
		printf("\n");
	}
	else if(direction=='r')						//print in reverse
	{
		printf("stack is:\n");
		struct Node *pp=squeue->last;				//begin with the last
		printf("%c ", iferror);
		while(pp!=NULL)
		{
			printf("\t%c\n",pp->val);
			pp=pp->prev;
		}
	}
	else
		fprintf(stderr,"Error, illegal direction <%c>",direction);
}
