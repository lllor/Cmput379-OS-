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
 
#define portnum 9999
#define FILE_SIZE 500 
#define BUFFER_SIZE 1024
 
int sendfile(int sock_desc);
 
int main()
{
	char name[30]={0};
	printf("请输入服务器的主机名或者ip\n");
	scanf("%s",name);
	struct hostent *h;
	//获取服务器信息
	h=gethostbyname(name);
	if(NULL==h)
	{
		perror("geyhostbyname");
		exit(1);
	}
	

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
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//=//*((struct in_addr *)h->h_addr_list[0]);
	
	if(connect(sock_desc,(struct sockaddr *)&server_addr,sizeof(server_addr)) <0)
	{
		perror("connect");
		exit(3);
	}
	
	while(1)
	{
		
		sendfile(sock_desc);
 
    }
	return 0;
	
}
int sendfile(int sock_desc)
{
    // 输入文件名 并放到缓冲区buffer中等待发送 
	int file_fp;
    char file_name[FILE_SIZE];  
	memset( file_name,0, sizeof(file_name) );
    printf("Please Input File Name On Server:   "); 
    
    scanf("%s", file_name); 
   
    char buffer[BUFFER_SIZE]; 
    memset( buffer,0, sizeof(buffer) );
    strncpy(buffer, file_name, strlen(file_name)>sizeof(buffer)?sizeof(buffer):strlen(file_name)); 
     
    // 向服务器发送buffer中的数据 
    if(write(sock_desc, buffer, sizeof(buffer)) < 0) 
    { 
       perror("Send File Name Failed:"); 
       exit(1); 
    } 
	
	if( strcmp(file_name,"null")==0 )
	{
		exit(1);
		close(sock_desc);
	}	
	 // 打开文件，准备写入 
     file_fp = open(file_name,O_CREAT|O_RDWR,0777); 
     if( file_fp<0 ) 
     { 
         printf("File:\t%s Can Not Open To Write\n", file_name); 
         exit(1); 
     } 
   
     // 从服务器接收数据到buffer中 
     // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止 
    int length = 0; 
	memset( buffer,0, sizeof(buffer) );
   
    while((length = read(sock_desc, buffer, sizeof(buffer))) > 0) 
    { 
        if( write( file_fp, buffer, length ) < length) 
        { 
            printf("File:\t%s Write Failed\n", file_name); 
            break; 
        } 
		if(length < sizeof(buffer))
		{
			break;
		}
        memset( buffer,0, sizeof(buffer) );
    } 
	 
	// 接收成功后，关闭文件，关闭socket 
     printf("Receive File:\t%s From Server IP Successful!\n", file_name); 
     close(file_fp); 	
}
