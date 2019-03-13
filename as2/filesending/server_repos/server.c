#define _POSIX_SOURCE
#define _GNU_SOURCE
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "squeue.h"
#include <netinet/in.h>
#include <pthread.h>
#include <openssl/md5.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <signal.h>
#include <sys/stat.h>
#define portnum 9999
#define FILE_SIZE 500 
#define BUFFER_SIZE 1024

int new_fd = 0; 
int serverSocket;
Squeue *map = NULL;
pthread_mutex_t lock;
int G_MUTEX = 1;
FILE *logfile;

void *net_thread(void * fd);
char *convertMD5(const char *str, int length);
int updatexml(char filename[],const char *md5);
void readxml();
void download(void * fd,char input[]);
void list(void * fd);
void update(void * fd,char input[]);
void delete(void * fd,char input[]);
//void createxml(char filename[], char *content);

void term(int signum)
{
   //rename("repository.xml", ".dedup");
   	xmlDocPtr doc;           //定义解析文件指针
	xmlNodePtr curNode;      //定义结点指针
	xmlChar *szKey;          //临时字符串变量
	char *szDocName;

	xmlChar *result = NULL;
	doc = xmlNewDoc(BAD_CAST "1.0");  //定义文档和节点指针

	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST "repository");    
	xmlDocSetRootElement(doc,root_node);        //设置根节点

	curNode = root_node;
   	
   	while((map)->first!=NULL)					//free the memort til the squeue is empty
	{
		printf("add new node\n");
		xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "file");
		xmlAddChild(curNode,node);
		xmlNewTextChild(node, NULL, BAD_CAST "hashname", BAD_CAST (map->first)->md5);
		xmlNewTextChild(node, NULL, BAD_CAST "knownas", BAD_CAST (map->first)->filename);


		leaveFront(map);
		//counter += 1;
	}
	int nRel = xmlSaveFile("repository.xml",doc);
	xmlFreeDoc(doc);
   	close(serverSocket);
   	exit(1);
}
int main()
{
	
	
	pid_t process_id = 0;
	pid_t sid = 0;
	// Create child process
	process_id = fork();
	// Indication of fork() failure
	if (process_id < 0)
	{
		printf("fork failed!\n");
		// Return failure in exit status
		exit(1);
	}
	// PARENT PROCESS. Need to kill it.
	if (process_id > 0)
	{
		printf("process_id of child process %d \n", process_id);
		// return success in exit status
		exit(0);
	}
	//unmask the file mode
	umask(0);
	//set new session
	sid = setsid();
	if(sid < 0)
	{
		// Return failure
		exit(1);
	}
	// Change the current working directory to root.
	chdir("./");
	// Close stdin. stdout and stderr
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	sleep(1);

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);


    initSqueue(&map);
	int checker = access(".dedup",0);
	if (checker == 0){
		readxml();	
	}
	//print(map,'f','r');
	//logfile = fopen("x.log","a+");
	int newSocket;
	struct sockaddr_storage serverStorgae;
	socklen_t addr_size;
	serverSocket=socket(AF_INET,SOCK_STREAM,0);
	if(-1==serverSocket)
	{
		perror("socket");
		exit(1);
	}
	
	struct sockaddr_in server_addr;   //struct sockaddr_in为结构体类型 ，server_addr为定义的结构体   
	server_addr.sin_family=AF_INET;   //Internet地址族=AF_INET(IPv4协议) 
	server_addr.sin_port=htons(portnum);  //将主机字节序转化为网络字节序 ,portnum是端口号
	(server_addr.sin_addr).s_addr=inet_addr("127.0.0.1");//IP地址
	memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);
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
	if(-1==listen(serverSocket,50)) //50是最大连接数，指服务器最多连接5个用户
	{
		perror("listen");
		exit(7);
	}
	//fprintf(logfile,"listening\n");
	
	pthread_t tid[50];
	int i = 0;
	int arrayInteger[50] = {0};
	pthread_mutex_init(&lock,NULL);

	while(1)
	{

		//pthread_mutex_init(&lock,NULL);

		addr_size = sizeof(serverStorgae);
		//newSocket = accept(serverSocket,(struct sockaddr *) &serverStorgae,&addr_size);
		arrayInteger[i] = accept(serverSocket,(struct sockaddr *) &serverStorgae,&addr_size);
		
		if(-1==newSocket)
		{
			perror("accept");
			exit(1);       //进行下一次循环
		}
		
		printf("new_fd=%d\n",newSocket);
		

		int ret = pthread_create(&tid[i],NULL,net_thread,&arrayInteger[i]);
		if(ret != 0){
			printf("Failed to create thread\n");
		}
		i++;
		if(i>=50){
			i=0;
			while(i<50){
				pthread_join(tid[i],NULL);
				i+=1;
			}
			i=0;
		}
		//pthread_mutex_destroy(&lock);
		
	}
	pthread_mutex_destroy(&lock);
	close(serverSocket);
	return 0;
 
}
 
 
 
void *net_thread(void * fd)
{
	//pthread_mutex_lock(&lock);
	pthread_detach(pthread_self()); //线程分离
	
	int newSocket=*((int *)fd);
	
	printf("%d\n",newSocket);
	
	char buffer[FILE_SIZE];
	while(1)
	{
		// recv函数接收数据到缓冲区buffer中 
        
        memset( buffer,0, sizeof(buffer) );		
        if(read(newSocket, buffer, sizeof(buffer)) < 0) 
        { 
           perror("Server Recieve Data Failed:"); 
           break; 
        } 
		

        char temp_buffer[5];
		memset( temp_buffer,'\0', sizeof(temp_buffer) );	
        strncpy(temp_buffer,buffer,4);
        
        //fprintf(logfile,"buffer is %s\n",buffer);
        
        if(strcmp(temp_buffer,"0x02") == 0){
        	if (G_MUTEX)
				pthread_mutex_lock(&lock);
        	
        	printf("ready update\n");
        	strncpy(temp_buffer,buffer+4,499-4);
        	printf("%s\n",temp_buffer);
        	update(fd,temp_buffer);
        	
        	if (G_MUTEX)
    			pthread_mutex_unlock(&lock);
        	memset( buffer,0, sizeof(buffer) );
        	continue;
        }
        if(strcmp(temp_buffer,"0x06") == 0){
        	if (G_MUTEX)
				pthread_mutex_lock(&lock);
        	
        	printf("ready download\n");
        	strncpy(temp_buffer,buffer+4,499-4);
        	printf("%s\n",temp_buffer);
        	download(fd,temp_buffer);
        	
        	if (G_MUTEX)
    			pthread_mutex_unlock(&lock);
    		memset( buffer,0, sizeof(buffer) );
    		continue;
        }
        if(strcmp(temp_buffer,"0x00") == 0){
        	if (G_MUTEX)
				pthread_mutex_lock(&lock);
        	
        	printf("ready list\n");
        	list(fd);
        	
        	if (G_MUTEX)
    			pthread_mutex_unlock(&lock);
        	memset( buffer,0, sizeof(buffer) );
    		continue;
        }
        if(strcmp(temp_buffer,"0x04") == 0){
        	if (G_MUTEX)
				pthread_mutex_lock(&lock);
            
            printf("ready delete\n");
            strncpy(temp_buffer,buffer+4,499-4);
            printf("%s\n",temp_buffer);
            delete(fd,temp_buffer);
            
            if (G_MUTEX)
    			pthread_mutex_unlock(&lock);
    		memset( buffer,0, sizeof(buffer) );
    		continue;
        }
        if(strcmp(temp_buffer,"0x08") == 0){
            write(newSocket,"0x09",sizeof("0x09"));
            memset( buffer,0, sizeof(buffer) );
            break;
        }
        memset( buffer,0, sizeof(buffer) );
        
     
		
        
	}
	//if (G_MUTEX)
    //	pthread_mutex_unlock(&lock);
	close(newSocket);
	
}
void delete(void * fd, char input[]){
    int newSocket=*((int *)fd);
    printf("%d\n",newSocket);
    int flag = 0;
	Squeue *new=malloc(sizeof(map));
	new->first=NULL;
	new->last=NULL;
	while((map)->first!=NULL)					//free the memort til the squeue is empty
	{
		
		if(strcmp((map->first)->filename,input) == 0){
			flag = 1;
			remove((map->first)->md5);
			leaveFront(map);
		}
		else{
			addBack(new,(map->first)->filename,(map->first)->content,(map->first)->md5);
			leaveFront(map);
		}
		
	}
	while((new)->first!=NULL)					//store all node form temp squeue into orignal squeue
	{
		addBack(map,(new->first)->filename,(new->first)->content,(new->first)->md5);
		leaveFront(new);
	}
	free(new);

	if(flag == 0){
        write(newSocket,"0xFF",sizeof("0xFF"));
    }
    else{
        write(newSocket,"0x05",sizeof("0x05"));
    }

}
void list(void *fd){
	int newSocket=*((int *)fd);
	char buffer[BUFFER_SIZE];
	printf("%d\n",newSocket);
	int i = 0;
	int counter = 0;
	Squeue *new=malloc(sizeof(map));
	new->first=NULL;
	new->last=NULL;
	while((map)->first!=NULL)					//free the memort til the squeue is empty
	{
		addBack(new,(map->first)->filename,(map->first)->content,(map->first)->md5);
		leaveFront(map);
		counter += 1;
	}
	char filename[counter][BUFFER_SIZE];
	while((new)->first!=NULL)					//store all node form temp squeue into orignal squeue
	{
		addBack(map,(new->first)->filename,(new->first)->content,(new->first)->md5);
		strcpy(filename[i],(new->first)->filename);
		i += 1;
		leaveFront(new);
	}
	free(new);

	unsigned int x = counter;
    unsigned char a[2];
    a[1] = (x>>8) & 0xFF;
    a[0] = x & 0xFF;
    if(write(newSocket,"0x01",sizeof("0x01")) < 0){
        write(newSocket,"0xFF",sizeof("0xFF"));
    }
    if(write(newSocket,a,2) < 0)
    {
        perror("Send list size Failed:"); 
        exit(1);
    }
    i = 0;
    while(counter>0){
        //printf("%s\n",name[counter]);
        write(newSocket,filename[i],sizeof(filename[i]));
        i += 1;
        counter -= 1;
    }
    return;
}
void update(void * fd,char input[]){
	int newSocket=*((int *)fd);
	char buffer[BUFFER_SIZE];
	memset(buffer,0,sizeof(buffer));
	
	printf("%d\n",newSocket);
	int flag = 0;
	Squeue *new=malloc(sizeof(map));
	new->first=NULL;
	new->last=NULL;
	while((map)->first!=NULL)					//free the memort til the squeue is empty
	{
		addBack(new,(map->first)->filename,(map->first)->content,(map->first)->md5);
		if(strcmp((map->first)->filename,input) == 0){
			flag = 1;
		}
		leaveFront(map);
	}
	while((new)->first!=NULL)					//store all node form temp squeue into orignal squeue
	{
		addBack(map,(new->first)->filename,(new->first)->content,(new->first)->md5);
		leaveFront(new);
	}
	free(new);

	if(flag == 1){
    	write(newSocket,"0xFF",sizeof("0xFF"));
    	return;
    }
	unsigned char size[4];
	int x;
	unsigned int length;
	int counter = 0;
	memset(size,0,sizeof(size));
	while((length = read(newSocket, size, sizeof(size))) > 0) 
	{
		counter ++;
		
		x = *(int *)size;
		//fprintf(logfile,"size is %d which is %d, counter : %d\n",length,x,counter);
		//fprintf(logfile,"%x %x %x %x \n",size[3],size[2],size[1],size[0]);
	 	if(x!=0){
	 		break;
	 	}
	 	memset(size,0, sizeof(size) );
	}
	char *data;
	data = (char *)malloc((x+1) * sizeof(char));
	memset(data,0,x+1);
	int RecvSize=0;

	while(x>0) 
    { 
    	RecvSize = recv(newSocket,data+RecvSize,x,0);
    	if(RecvSize == -1){
    		perror("Recieve File Content Failed:"); 
            write(newSocket,"0xFF",sizeof("0xFF"));
            exit(1);
    	}

    	x = x- RecvSize;
    }
	
	char *content;
	//fprintf(logfile,"Data: %s\n",data);
	char *md5 = convertMD5(data,strlen(data));
	char *filename;
	filename = malloc(strlen(input)+1);
	strcpy(filename,input);
	addBack(map,input,data,md5);
	//printf("%s\n",md5);

	

	
    //char flag[5] = "0x03";
    
    
	FILE *fp = fopen(md5, "ab");
	if (fp != NULL)
	{
		fputs(data, fp);
		fclose(fp);
	}
	write(newSocket,"0x03",sizeof("0x03"));
    
    free(md5);
	free(data);
	free(filename);
	printf("update end\n");
	return;

}
void download(void * fd,char input[]){
	printf("in download\n");
	int newSocket=*((int *)fd);
	char buffer[BUFFER_SIZE];
	printf("%d\n",newSocket);
	int flag = 0;
	char *filename;
	char *content;
	char *md5;
	Squeue *new=malloc(sizeof(map));
	new->first=NULL;
	new->last=NULL;
	while((map)->first!=NULL)					//free the memort til the squeue is empty
	{
		addBack(new,(map->first)->filename,(map->first)->content,(map->first)->md5);
		if(strcmp((map->first)->filename,input) == 0){
			flag = 1;
			filename = malloc(1+strlen((map->first)->filename));
			content = malloc(1+strlen((map->first)->content));
			md5 = malloc(1+strlen((map->first)->md5));
			strcpy(filename,(map->first)->filename);
			strcpy(content,(map->first)->content);
			strcpy(md5,(map->first)->md5);
		}
		leaveFront(map);
	}
	while((new)->first!=NULL)					//store all node form temp squeue into orignal squeue
	{
		addBack(map,(new->first)->filename,(new->first)->content,(new->first)->md5);
		leaveFront(new);
	}
	free(new);

	printf("%s\n",filename);
	printf("%s\n",content);

	if(flag == 1){
		write(newSocket,"0x07",sizeof("0x07"));
	}
	else{
		write(newSocket,"0xFF",sizeof("0xFF"));
	}

    unsigned int num = strlen(content);
    unsigned int x;
    unsigned char a[4];

    a[3] = (num>>24) & 0xFF;
    a[2] = (num>>16) & 0xFF;
    a[1] = (num>>8) & 0xFF;
    a[0] = num & 0xFF;
    printf("%x %x %x %x \n",a[3],a[2],a[1],a[0]);
    x = *(int *)a;
    printf("%d\n", x);


    memset(buffer,0,sizeof(buffer));
    strncpy(buffer,a,4);
    printf("%s\n",buffer);
    int SendSize = 0;
    if(write(newSocket,buffer,4) < 0)
    {
        perror("Send File size Failed:"); 
        exit(1);
    }
    
    while (num>0){
        SendSize = send(newSocket,content + SendSize,num,0);
        if(SendSize == -1){
            perror("Send File Content Failed:"); 
            exit(1);
        }
        num = num-SendSize;
    }
    free(filename);
    free(content);
    free(md5);
    return;   
}
char *convertMD5(const char *str,int length){
	int n;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char*)malloc(33);

    MD5_Init(&c);

    while (length > 0) {
        if (length > 512) {
            MD5_Update(&c, str, 512);
        } else {
            MD5_Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    MD5_Final(digest, &c);

    for (n = 0; n < 16; ++n) {
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
    }

    return out;
}
int updatexml(char filename[],const char* md5){
	xmlDocPtr doc;           //定义解析文件指针
    xmlNodePtr curNode;      //定义结点指针
    xmlChar *szKey;          //临时字符串变量
    char *szDocName;
    
    xmlChar *result = NULL;
    doc = xmlNewDoc(BAD_CAST "1.0");  //定义文档和节点指针
 
    xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST "repository");    
    xmlDocSetRootElement(doc,root_node);        //设置根节点
   
    curNode = root_node;

	printf("add new node\n");
	xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "file");
	xmlAddChild(curNode,node);
	xmlNewTextChild(node, NULL, BAD_CAST "hashname", BAD_CAST md5);
	xmlNewTextChild(node, NULL, BAD_CAST "knownas", BAD_CAST filename);
    
    
    int nRel = xmlSaveFile("repository.xml",doc);


    xmlFreeDoc(doc);
	return 1;
}
void readxml(){
 	xmlDocPtr doc;           //定义解析文件指针
    xmlNodePtr curNode;      //定义结点指针
    xmlChar *szKey;          //临时字符串变量
    
    rename(".dedup","repository.xml");
    doc = xmlReadFile("repository.xml","GB2312",XML_PARSE_RECOVER);
    //解析文件
    //检查解析文档是否成功，如果不成功，libxml将报错并停止解析。
    //一个常见错误是不适当的编码，XML标准文档除了用UTF-8或UTF-16外还可用其它编码保存
    if (NULL == doc) {
        fprintf(stderr,"Document not parsed successfully.");
        return;
    }
    //获取根节点
    curNode = xmlDocGetRootElement(doc);
    if (NULL == curNode) {
        fprintf(stderr,"empty document");
        xmlFreeDoc(doc);
        return;
    }
    curNode = curNode->xmlChildrenNode;
    xmlNodePtr propNodePtr = curNode;
    xmlNodePtr tepNode;      //定义结点指针
    
    //char *filename;
    //char *buffer;
    //char *md5;
    long length;
    while(curNode != NULL) {
        //取出节点中的内容
        if ((!xmlStrcmp(curNode->name, (const xmlChar *) "file"))) {
            
            tepNode = curNode;
            curNode = curNode->children;
            while(curNode !=NULL){
                char *buffer;
                szKey = xmlNodeGetContent(curNode);
                FILE * f = fopen (szKey, "rb");

				if (f)
				{
					fseek (f, 0, SEEK_END);
    				length = ftell (f);
    				fseek (f, 0, SEEK_SET);
    				buffer = malloc (length);
  	 				if (buffer)
  					{
    					fread (buffer, 1, length, f);
  					}
  					fclose (f);
				}
                addBack(map,szKey,buffer,xmlNodeGetContent(tepNode->xmlChildrenNode));
                free(buffer);
                
                curNode = curNode->next;
            }
            curNode = tepNode;
           
            xmlFree(szKey);
        }
        
        curNode = curNode->next;

    }
    int nRel = xmlSaveFile("copy.xml",doc);
    xmlFreeDoc(doc);
}