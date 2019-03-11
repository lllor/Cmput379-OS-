#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int counter = 0;

void *compute()
{
	counter++;
	printf("Counter value: %d\n", counter);
}

int main() 
{
	pthread_t thread[20];

	for (int i = 0; i < 10; i ++)
		pthread_create(&thread[i], NULL, compute, NULL);

	for (int i = 0; i < 10; i ++)
		pthread_join( thread[i], NULL);


	return(0);
}