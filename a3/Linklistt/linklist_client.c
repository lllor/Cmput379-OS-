#include "linklist.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
	Linklist *l1 = NULL;

	initLinklist(&l1);
	addBack(l1, 10);

	addBack(l1, 20);
	addBack(l1, 30);

	removeCurrent(l1,20);
	struct Node *pp=l1->first;				//begin with the first element
		//printf("%c ", iferror);
	while(pp!=NULL)						//til the end
	{
		printf("%d\n",pp->pagenum);
		pp=pp->next;
	}
	destroy(&l1);
}