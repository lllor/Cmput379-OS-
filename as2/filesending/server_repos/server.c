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
#include <pthread.h>
 
#define portnum 9999
#define FILE_SIZE 500 
#define BUFFER_SIZE 1024
int new_fd = 0; 
void *net_thread(void * fd);
void download(void * fd,char input[]);
void update(void * fd,char input[]);
int main()
{
	int serverSocket, newSocket;
	struct sockaddr_storage serverStorgae;
	socklen_t addr_size;
	//初始化套接字
	serverSocket=socket(AF_INET,SOCK_STREAM,0);
	if(-1==serverSocket)
	{
		perror("socket");
		exit(1);
	}
	//绑定端口和ip;
	struct sockaddr_in server_addr;   //struct sockaddr_in为结构体类型 ，server_addr为定义的结构体   
	server_addr.sin_family=AF_INET;   //Internet地址族=AF_INET(IPv4协议) 
	server_addr.sin_port=htons(portnum);  //将主机字节序转化为网络字节序 ,portnum是端口号
	(server_addr.sin_addr).s_addr=inet_addr("127.0.0.1");//IP地址
	memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);
	// 设置套接字选项避免地址使用错误
	printf("i'm here");
	int on=1;
	if((setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)
	{
		perror("setsockopt failed");
		exit(EXIT_FAILURE);
	}

	if(-1==bind(serverSocket,(struct sockaddr *)&server_addr,sizeof(server_addr)))  //套接字与端口绑定
	{
		perror("bind");
		exit(6);
	}
	printf("bind surssess");
	//开启监听
	if(-1==listen(serverSocket,50)) //50是最大连接数，指服务器最多连接5个用户
	{
		perror("listen");
		exit(7);
	}
	printf("listening");
	pthread_t tid[60];
	int i = 0;
	while(1)
	{
		addr_size = sizeof(serverStorgae);
		newSocket = accept(serverSocket,(struct sockaddr *) &serverStorgae,&addr_size);
		
		if(-1==newSocket)
		{
			perror("accept");
			exit(1);       //进行下一次循环
		}
		
		printf("new_fd=%d\n",newSocket);
		
		// 打开文件，存入客户端的文件描述符
	    FILE *file_fp = fopen("01.file_fp", "w+"); 
        if(NULL == file_fp) 
        { 
             printf(" open 01.file_fp failure\n" ); 
             exit(1); 
        } 
	    else 
	    {
		     int a=newSocket;
		     fprintf(file_fp,"%d\n",newSocket);
		     fclose(file_fp);
	    }
			
		//int pthread_id;
		//int ret = pthread_create((pthread_t *)&pthread_id,NULL,net_thread,(void *)&new_fd);
		int ret = pthread_create(&tid[i],NULL,net_thread,&newSocket);
		if(ret != 0){
			printf("Failed to create thread\n");
		}
		i++;
		if(i>=50){
			i=0;
			while(i<50){
				pthread_join(tid[i++],NULL);
			}
			i=0;
		}
		
	}
	close(serverSocket);
	return 0;
 
}
 
 
 
void *net_thread(void * fd)
{
	//pthread_detach(pthread_self()); //线程分离
	int newSocket=*((int *)fd);
	int file2_fp;
	
	while(1)
	{
		// recv函数接收数据到缓冲区buffer中 
        char buffer[BUFFER_SIZE];
        memset( buffer,0, sizeof(buffer) );		
        if(read(newSocket, buffer, sizeof(buffer)) < 0) 
        { 
           perror("Server Recieve Data Failed:"); 
           break; 
        } 
		
        // 然后从buffer(缓冲区)拷贝到file_name中 
        //char file_name[FILE_SIZE]; 
        char temp_buffer[FILE_SIZE];
		//memset( file_name,0, sizeof(file_name) );
		memset( temp_buffer,0, sizeof(temp_buffer) );	
        strncpy(temp_buffer, buffer, strlen(buffer)>FILE_SIZE?FILE_SIZE:strlen(buffer));
        memset( buffer,0, sizeof(buffer) );
        printf("%s\n",temp_buffer);
        switch(temp_buffer[0])
      	{
      		case 'l':	
      					break;
      		case 'u':	update(fd,temp_buffer);	
      					break;
      		case 'd':	download(fd,temp_buffer);
      					break;
      		case 'r':
      					break;
      		case 'q':
      					break;
      	}
    //strncpy(file_name, buffer, strlen(buffer)>FILE_SIZE?FILE_SIZE:strlen(buffer)); 
		
        
	}
	close(newSocket);
}
void update(void * fd,char input[]){
	int newSocket=*((int *)fd);
	int file2_fp;
	char buffer[BUFFER_SIZE];
	char file_name[FILE_SIZE];
	
	memset( file_name,0, sizeof(file_name) );
	strncpy(file_name,input+2,499-2);

	int file_fp = open(file_name,O_CREAT|O_RDWR,0777); 
    if(file_fp<0 ) 
    { 
        printf("File:\t%s Can Not Open To Write\n", file_name); 
        exit(1); 
    } 
   
     // 从客户端接收数据到buffer中 
     // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止 
    int length = 0; 
	memset( buffer,0, sizeof(buffer) );
   
    while((length = read(newSocket, buffer, sizeof(buffer))) > 0) 
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
    printf("File:%s update Successful!\n", file_name);

}
void download(void * fd,char input[]){
	int newSocket=*((int *)fd);
	int file2_fp;
	char buffer[BUFFER_SIZE];
	char file_name[FILE_SIZE];
	memset( file_name,0, sizeof(file_name) );
	strncpy(file_name,input+2,499-2);

	printf("%s\n", file_name); 
		
	if( strcmp(file_name,"null")==0 )
    {
	   return;
	   close(newSocket);
    }
	
	  // 打开文件并读取文件数据 
     file2_fp = open(file_name,O_RDONLY,0777); 
     if(file2_fp<0) 
     { 
        printf("File:%s Not Found\n", file_name); 
     } 
     else 
     { 
        int length = 0; 
		memset( buffer,0, sizeof(buffer) );
        // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止 
        while( (length = read(file2_fp, buffer, sizeof(buffer))) > 0  )    
        {   
            if( write(newSocket, buffer, length) < 0) 
            { 
                printf("Send File:%s Failed.\n", file_name); 
                break; 
            } 
            memset( buffer,0, sizeof(buffer) );
        } 
          // 关闭文件 
          close(file2_fp); 
          printf("File:%s Transfer Successful!\n", file_name); 
     }
     return;   
}