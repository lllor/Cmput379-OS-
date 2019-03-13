#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
 
#define FILE_SIZE 500 
#define BUFFER_SIZE 1024

int deletefile(int sock_desc, char input[]);
int listfile(int sock_desc);
int quit(int sock_desc);
int downloadfile(int sock_desc,char input[]);
int updatefile(int sock_desc,char input[]);
int main(int argc, char *argv[])
{
    if(argc < 3 || argc > 3){
        printf("wrong argument number");
        exit(1);
    }
    char *ServerAddress = argv[1];
    int portnum = atoi(argv[2]);
    char name[30]={0};
	
	
    //初始化套接字
	  int sock_desc=socket(AF_INET,SOCK_STREAM,0);//sock_desc
	  if(-1==sock_desc)
	  {
		    perror("socket");
		    exit(2);
	  }
	  struct sockaddr_in server_addr;
	  server_addr.sin_family=AF_INET;
	  server_addr.sin_port=htons(portnum);
	  server_addr.sin_addr.s_addr = inet_addr(ServerAddress);//=//*((struct in_addr *)h->h_addr_list[0]);
	
	  if(connect(sock_desc,(struct sockaddr *)&server_addr,sizeof(server_addr)) <0)
	  {
		    perror("connect");
		    exit(3);
	  }
	  char input[500] = {0};
	  while(1)
 	  {
		    char ch;
		    int i = 0;
		    while((ch = getchar()) != '\n') {
          	input[i] = ch;
        	  i++;
        }
      	switch(input[0])
      	{
      	  	case 'l':{
                      listfile(sock_desc);
                      break;
                    }
      		  case 'u':{
                      updatefile(sock_desc,input);
                      break;
                    }	
      		  case 'd':	{
                      downloadfile(sock_desc,input);
      					      break;
                    }
      		  case 'r':{
                      deletefile(sock_desc,input);
                      //printf("back\n");
                      break;
                    }
      			case 'q':{
                      quit(sock_desc);
                      break;
                    }
      					
      	}
		//downloadfile(sock_desc);

	 	    memset(input,0,sizeof(input));
 
    }
	  return 0;
	
}
int quit(int sock_desc){
    char buffer[1024] = "0x08";
    if(write(sock_desc,buffer,sizeof(buffer)) < 0)
    {
        perror("Send File Name Failed:"); 
        exit(1);
    }
    unsigned int length;
    int counter = 0;
    char flag[5]={0};
    while((length = read(sock_desc, flag, sizeof(flag))) > 0) 
    {
      if(strcmp(flag,"0x09") == 0){
        printf("OK\n"); 
        exit(1);
      }
      memset(flag,0, sizeof(flag) );
    }
}
int deletefile(int sock_desc,char input[])
{
    char file_name[FILE_SIZE];
    char buffer[1024] = "0x04";
    
    memset(file_name,0,sizeof(file_name));
    strncpy(file_name,input+2,499-2);
    strcat(buffer,input+2);
    strcat(buffer,"\0");
    if(write(sock_desc,buffer,sizeof(buffer)) < 0)
    {
        perror("Send File Name Failed:"); 
        exit(1);
    }
    unsigned int length;
    int counter = 0;
    char flag[5]={0};
    while((length = read(sock_desc, flag, sizeof(flag))) > 0) 
    {
      printf("size is %d which %s\n",length,flag);
      if(strcmp(flag,"0x05") == 0){
        printf("OK\n"); 
        break;
      }
      if(strcmp(flag,"0xFF") == 0){
        printf("SERROR file not find in server repository\n");
        return 1;
      }
      memset(flag,0, sizeof(flag) );
    }
    //printf("end");
    return 1;

}
int listfile(int sock_desc){
    char buffer[BUFFER_SIZE] = "0x00";
    if(write(sock_desc,buffer,sizeof(buffer)) < 0)
    {
        perror("Send command Failed:"); 
        exit(1);
    }

    memset(buffer,0,sizeof(buffer));
    unsigned char size[2]={0};
    int x =-1;
    unsigned int length;
    int counter = 0;
    char flag[5]={0};
    while((length = read(sock_desc, flag, sizeof(flag))) > 0) 
    {
      //printf("size is %d which %s\n",length,size);
      if(strcmp(flag,"0x01") == 0){
        //printf("File:%s list Successful!\n", file_name); 
        break;
      }
      if(strcmp(flag,"0xFF") == 0){
        //printf("File:%s list Failed!\n", file_name); 
        return 1;
      }
      memset(flag,0, sizeof(flag) );
    }

    while((length = read(sock_desc, size, sizeof(size))) > 0) 
    {
      
        x = size[0] | size[1] << 8;
        //printf("size is %d which is %d, counter : %d\n",length,x,counter);
        if(x!=-1){
          break;
        }
        memset(size,0, sizeof(size) );
    }
    
    char file_name[BUFFER_SIZE];
    counter = 0;
    while(x>0){
        read(sock_desc,file_name,BUFFER_SIZE);
        printf("OK+ %s\n",file_name);
        counter += 1;
        x -= 1;
        memset(file_name,0,sizeof(file_name));
    }
    //printf("read end");
    //for(int i = 0;i<x; i += 1){
    //    printf("OK+ %s\n",file_name[i]);
   // }
    printf("OK\n");

}
int updatefile(int sock_desc,char input[])
{
	  char file_name[FILE_SIZE];
	  char buffer[1024] = "0x02";
	  
    memset(file_name,0,sizeof(file_name));
  	strncpy(file_name,input+2,499-2);
    
    FILE *inFile = fopen (file_name, "r");
  	if(inFile == NULL) 
    { 
        printf("File:%s Not Found\n", file_name); 
        return 0;
    } 
    fseek(inFile, 0L, SEEK_END);
    int sz = ftell(inFile);//size to send
    fseek(inFile, 0L, SEEK_SET);//set the file pointer back to beginning
    
    unsigned int num = sz;
    unsigned int x;
    unsigned char a[4];

    a[3] = (num>>24) & 0xFF;
    a[2] = (num>>16) & 0xFF;
    a[1] = (num>>8) & 0xFF;
    a[0] = num & 0xFF;
    printf("%x %x %x %x \n",a[3],a[2],a[1],a[0]);
    x = *(int *)a;
    printf("%d\n", x);

    strcat(buffer,input+2);
    strcat(buffer,"\0");
    if(write(sock_desc,buffer,sizeof(buffer)) < 0)
    {
        perror("Send File Name Failed:"); 
        exit(1);
    }
    memset(buffer,0,sizeof(buffer));
    strncpy(buffer,a,4);
    printf("%s\n",buffer);
    int SendSize = 0;
    if(write(sock_desc,buffer,4) < 0)
    {
        perror("Send File Name Failed:"); 
        exit(1);
    }
    
    int length = sz;
    char *data;
    data = (char *)malloc(length*sizeof(char));
    memset(data,0, sizeof(data) );
    fread(data,1,length,inFile);
    printf("%s",data);
    int rest = num;
    
    while(rest > 0){
        SendSize = send(sock_desc,data + SendSize,rest,0);
        if(SendSize == -1){
            perror("Send File Content Failed:"); 
            exit(1);
        }
        rest = rest-SendSize;
    }
    
    char size[5]={0};
    while((length = read(sock_desc, size, sizeof(size))) > 0) 
    {
      //printf("size is %d which %s\n",length,size);
      if(strcmp(size,"0x03") == 0){
        //printf("File:%s update Successful!\n", file_name); 
        printf("OK\n");
	break;
      }
      if(strcmp(size,"0xFF") == 0){
        printf("File:%s update Failed!\n", file_name); 
        break;
      }
      memset(size,0, sizeof(size) );
    }
    
    
    return 1;
}
int downloadfile(int sock_desc,char input[])
{
    char file_name[FILE_SIZE];
    char buffer[1024] = "0x06";
    
    memset(file_name,0,sizeof(file_name));
    strncpy(file_name,input+2,499-2);
    strcat(buffer,input+2);
    strcat(buffer,"\0");
    if(write(sock_desc,buffer,sizeof(buffer)) < 0)
    {
        perror("Send File Name Failed:"); 
        exit(1);
    }

    //char buffer[BUFFER_SIZE];
    memset(buffer,0,sizeof(buffer));
    unsigned char size[4]={0};
    int x;
    unsigned int length;
    int counter = 0;
    char flag[5] = {0};
    
    while((length = read(sock_desc, flag, sizeof(flag))) > 0) 
    {
      printf("size is %d which %s\n",length,flag);
      if(strcmp(flag,"0x07") == 0){
        //printf("File:%s download Successful!\n", file_name); 
        break;
      }
      if(strcmp(flag,"0xFF") == 0){
        printf("File:%s download Failed!\n", file_name); 
        return 1;
      }
      memset(flag,0, sizeof(flag) );
    }
    
    while((length = read(sock_desc, size, sizeof(size))) > 0) 
    {
      counter ++;
      
      x = *(int *)size;
      printf("size is %d which is %d, counter : %d\n",length,x,counter);
      printf("%x %x %x %x \n",size[3],size[2],size[1],size[0]);
      if(x!=0){
        break;
      }
      memset(size,0, sizeof(size) );
    }
    // memset(buffer,0,BUFFER_SIZE);
    char *data;
    data = (char *)malloc((x+1) * sizeof(char));
    int RecvSize=0;
    //char content [x]  
    //printf("size is %d---------------------------------------\n",x);
    while(x>0) 
      { 
        RecvSize = recv(sock_desc,data+RecvSize,x,0);
        if(RecvSize == -1){
          perror("Recieve File Content Failed:"); 
              exit(1);
        }

        x = x- RecvSize;
      }
    
    //printf("%s\n",data);
    
    FILE *fp = fopen(file_name, "ab");
    if (fp != NULL)
    {
        fputs(data, fp);
        fclose(fp);
    }
    free(data);
    printf("OK\n");
    //printf("File:%s update Successful!\n", file_name); 
}
