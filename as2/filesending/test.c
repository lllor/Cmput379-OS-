#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
unsigned char i;
int main() {

	int x = 40;
	int num;
	unsigned char res[2]; /* two bytes of hex = 4 characters, plus NULL terminator */
	res[0] = x & 0xFF;
	res[1] = (x >> 8) & 0xFF;
	num = res[0] | res[1] << 8;
	printf("%d\n",num);

	
  //fseek(inFile, 0L, SEEK_SET);//set the file pointer back to beginning
 // 	FILE *inFile = fopen ("1.txt", "r");
 // 	char *data;
 // 	int *length;
 // 	fseek(inFile, 0L, SEEK_END);
 //    long int sz = ftell(inFile);
 //    *length = ftell(inFile);
 //    data = (char *)malloc((*length+1) * sizeof(char));
 //    fseek(inFile, 0L, SEEK_SET);
 //    *length = fread(data, 1, *length, inFile);
 //    data[*length] = '\0';
 //    fclose(inFile);
 //    int sendSize = 0;
 //    while (sz>0){
 //    	printf()
 //    }
 //    //fread(buffer,0, sizeof(buffer),fp)
 //    //printf("%s\n",data);
 //    //printf("%d\n",*length);
	// free(data);
	// if (fork() == 0){
	// 	//child process
	// 	for (i=1; i<=8; i++){
	// 		printf("This is child process\n");
	// 	}
	// 	//exit(0);
	// }else{
	// 	//parent process
	// 	//wait();
	// 	for(i=1; i<=8; i++){
	// 		printf("This is parent process\n");
	// 	}
	// }
 
	// 
	

		//printf("%d\n",fs);
		//if(fs){printf("*\n");}
 		//if (pid==0) { printf("child *\n"); }
 		//if (pid>0) {printf("parent!\n");}

}
