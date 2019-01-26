#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include <unistd.h>

#ifdef SINGLE
void testS(){
	printf("eee");
}
#else

#endif

int main(){
	printf("aa");
	testS();
	
	return 0;
}
