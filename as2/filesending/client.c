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


int downloadfile(int sock_desc,char input[]);
int updatefile(int sock_desc,char input[]);
int main()
{
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
	  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//=//*((struct in_addr *)h->h_addr_list[0]);
	
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
      	  	case 'l':break;
      		  case 'u':{
                      updatefile(sock_desc,input);
                      break;
                    }	
      		  case 'd':	{
                      downloadfile(sock_desc,input);
      					      break;
                    }
      		  case 'r':
      					break;
      	  	case 'q':
      					break;
      	}
		//downloadfile(sock_desc);

	 	    memset(input,0,sizeof(input));
 
    }
	  return 0;
	
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
    int SendSize = 3;
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
    
    while (num>0){
        strncpy(buffer,data,sizeof(buffer));

        SendSize = send(sock_desc,buffer,strlen(buffer),0);
        if(SendSize == -1){
            perror("Send File Content Failed:"); 
            exit(1);
        }
        num = num-SendSize;
    }

    
    

    
    

    

    
    // int SendSize = 0; 
    //memset( buffer,0, BUFFER_SIZE);
    

    // int read_len;
    // int send_len;
    // //while( (length = fread(buffer,0, sizeof(buffer),fp)) > 0  )
    // while ((read_len = fread(buffer,sizeof(char),BUFFER_SIZE,inFile))>0)    
    // { 
    //     printf("buffer is:%s\n",buffer);
    //     send_len = send(sock_desc, buffer, read_len,0);
    //     if(send_len< 0) 
    //     { 
    //         printf("Send File:%s Failed.\n", file_name); 
    //         break; 
    //     } 
    //     memset( buffer,0, BUFFER_SIZE );
    // } 


    // free(data);
    // fclose(inFile); 
    // printf("File:%s update Successful!\n", file_name); 
    
    return 1;
}
int downloadfile(int sock_desc,char input[])
{
    
    char file_name[FILE_SIZE];
    memset( file_name,0, sizeof(file_name) );
    
    //strncpy(file_name,input+2,499-2);
    strcpy(file_name,input);
    // 输入文件名 并放到缓冲区buffer中等待发送 
	  int file_fp;
     
	
    //printf("Please Input File Name On Server:   "); 
    //scanf("%s", file_name); 
   
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
    file_fp = open(file_name+2,O_CREAT|O_RDWR,0777); 
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
