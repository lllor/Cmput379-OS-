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
#include <openssl/md5.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define portnum 9999
#define FILE_SIZE 500 
#define BUFFER_SIZE 1024
int new_fd = 0; 
void *net_thread(void * fd);
char *convertMD5(const char *str, int length);
int updatexml(char filename[],const char *md5,const char *data);
void download(void * fd,char input[]);
void list(void * fd);
void update(void * fd,char input[]);
void delete(void * fd,char input[]);
//void createxml(char filename[], char *content);
void term(int signum)
{
   rename("repository.xml", ".dedup");
}
int main()
{
	int checker = access(".dedup",0);
	if (checker == 0){
		rename(".dedup","repository.xml");
	}
	else{
		xmlChar *result = NULL;
    	int size = 0;
    	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");  //定义文档和节点指针
 		char name[7] = "hello";
    	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST "repository");    
    	xmlDocSetRootElement(doc,root_node);
    	xmlNewTextChild(root_node, NULL, BAD_CAST "newNode1", BAD_CAST name);
    	xmlSaveFile("repository.xml",doc);
    	xmlFreeDoc(doc);
	}


    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);

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
        printf("buffer is %s\n",buffer);
        
        if(strcmp(temp_buffer,"0x02") == 0){
        	printf("ready update\n");
        	strncpy(temp_buffer,buffer+4,499-4);
        	printf("%s\n",temp_buffer);
        	update(fd,temp_buffer);
        	continue;
        }
        if(strcmp(temp_buffer,"0x06") == 0){
        	printf("ready download\n");
        	strncpy(temp_buffer,buffer+4,499-4);
        	printf("%s\n",temp_buffer);
        	download(fd,temp_buffer);
        }
        if(strcmp(temp_buffer,"0x00") == 0){
        	printf("ready list\n");
        	list(fd);
        }
        if(strcmp(temp_buffer,"0x04") == 0){
            printf("ready delete\n");
            strncpy(temp_buffer,buffer+4,499-4);
            printf("%s\n",temp_buffer);
            delete(fd,temp_buffer);
        }
        if(strcmp(temp_buffer,"0x08") == 0){
            write(newSocket,"0x09",sizeof("0x09"));
            break;
        }
        memset( buffer,0, sizeof(buffer) );
        
     
		
        
	}
	close(newSocket);
}
void delete(void * fd, char input[]){
    int newSocket=*((int *)fd);
    xmlDocPtr doc;           //定义解析文件指针
    xmlNodePtr curNode;      //定义结点指针
    xmlChar *szKey;          //临时字符串变量
    char *szDocName;
    
    doc = xmlReadFile("repository.xml","GB2312",XML_PARSE_RECOVER);
    
    if (NULL == doc) {
        fprintf(stderr,"Document not parsed successfully.");
        exit(1);
        //return -1;
    }
    //获取根节点
    curNode = xmlDocGetRootElement(doc);
    if (NULL == curNode) {
        fprintf(stderr,"empty document");
        xmlFreeDoc(doc);
        exit(1);
    }

    curNode = curNode->xmlChildrenNode;
    xmlNodePtr propNodePtr = curNode;
    xmlNodePtr tepNode;
    //int find_file = 0;
    int flag = 0;
    int counter = 0;
    while(curNode != NULL) {
        //取出节点中的内容
        if ((!xmlStrcmp(curNode->name, (const xmlChar *) "file"))) {
            tepNode = curNode;
            curNode = curNode->children;
            while(curNode !=NULL){
                szKey = xmlNodeGetContent(curNode);
                if(strcmp(szKey,input) == 0){
                    remove(xmlNodeGetContent(tepNode->xmlChildrenNode));
                    flag = 1;
                }
                
                curNode = curNode->next;
            }
            curNode = tepNode;
            
            xmlFree(szKey);
        }
        if(flag == 1){
            counter += 1;
            tepNode = curNode -> next;
            xmlUnlinkNode(curNode);
            xmlFreeNode(curNode);
            curNode = tepNode;
            flag = 0;
            continue;
        }
        
        curNode = curNode->next;

    }

    if(counter == 0){
        write(newSocket,"0xFF",sizeof("0xFF"));
    }
    else{
        write(newSocket,"0x05",sizeof("0x05"));
    }

    int nRel = xmlSaveFile("repository.xml",doc);
    if (nRel != -1)
    {
        printf("一个xml文档被创建，写入%d个字节\n", nRel);
    }
    //释放文档内节点动态申请的内存
    xmlFreeDoc(doc);

}
void list(void *fd){
	int newSocket=*((int *)fd);
	xmlDocPtr doc;           //定义解析文件指针
    xmlNodePtr curNode;      //定义结点指针
    xmlChar *szKey;          //临时字符串变量
    char *szDocName;
    int flag = 0;
    
    doc = xmlReadFile("repository.xml","GB2312",XML_PARSE_RECOVER);
    
    if (NULL == doc) {
        fprintf(stderr,"Document not parsed successfully.");
        exit(1);
        //return -1;
    }
    //获取根节点
    curNode = xmlDocGetRootElement(doc);
    if (NULL == curNode) {
        fprintf(stderr,"empty document");
        xmlFreeDoc(doc);
        exit(1);
    }

    curNode = curNode->xmlChildrenNode;
    xmlNodePtr propNodePtr = curNode;
    xmlNodePtr tepNode;
    int num =0;
    int counter = 0;
    while(curNode != NULL) {
        //取出节点中的内容
        if ((!xmlStrcmp(curNode->name, (const xmlChar *) "file"))) {
            tepNode = curNode;
            curNode = curNode->children->next;
            while(curNode !=NULL){
                num += 1;
                curNode = curNode->next;
            }
            curNode = tepNode;
        }
    curNode = curNode->next;
	}
	char name[num][BUFFER_SIZE];
	curNode = propNodePtr;
	while(curNode != NULL) {
        if ((!xmlStrcmp(curNode->name, (const xmlChar *) "file"))) {
            tepNode = curNode;
            curNode = curNode->children->next;
            while(curNode !=NULL){
                szKey = xmlNodeGetContent(curNode);
                strcpy(name[counter],szKey);
                counter +=1;
                curNode = curNode->next;
            }
            curNode = tepNode;
            xmlFree(szKey);
        }
    	curNode = curNode->next;
	}
    
    unsigned int x = num;
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
    counter = 0;
    while(num>0){
        //printf("%s\n",name[counter]);
        write(newSocket,name[counter],sizeof(name[counter]));
        counter += 1;
        num -= 1;
    }
    //printf("end\n");
    return;
}
void update(void * fd,char input[]){
	int newSocket=*((int *)fd);
	char buffer[BUFFER_SIZE];
	unsigned char size[4]={0};
	int x;
	unsigned int length;
	int counter = 0;
	while((length = read(newSocket, size, sizeof(size))) > 0) 
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
	printf("size is %d---------------------------------------\n",x);
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
	
	printf("%s\n",data);
	char *md5 = convertMD5(data,strlen(data));
	printf("%s\n",md5);
	updatexml(input,md5,data);

	free(md5);
	free(data);
    char flag[5] = "0x03";
    write(newSocket,flag,sizeof(flag));

	
 //    printf("File:%s update Successful!\n", file_name);
    return;

}
void download(void * fd,char input[]){
	printf("in download\n");
	int newSocket=*((int *)fd);
	char buffer[BUFFER_SIZE];
	
	xmlDocPtr doc;           //定义解析文件指针
    xmlNodePtr curNode;      //定义结点指针
    xmlChar *szKey;          //临时字符串变量
    char *szDocName;
    
    doc = xmlReadFile("repository.xml","GB2312",XML_PARSE_RECOVER);
    
    if (NULL == doc) {
        fprintf(stderr,"Document not parsed successfully.");
        exit(1);
        //return -1;
    }
    //获取根节点
    curNode = xmlDocGetRootElement(doc);
    if (NULL == curNode) {
        fprintf(stderr,"empty document");
        xmlFreeDoc(doc);
        exit(1);
    }

    curNode = curNode->xmlChildrenNode;
    xmlNodePtr propNodePtr = curNode;
    xmlNodePtr tepNode;
    //int find_file = 0;
    char md5[33];
    while(curNode != NULL) {
        //取出节点中的内容
        if ((!xmlStrcmp(curNode->name, (const xmlChar *) "file"))) {
            tepNode = curNode;
            curNode = curNode->children;
            while(curNode !=NULL){
                szKey = xmlNodeGetContent(curNode);
                //printf()
                if(strcmp(szKey,input) == 0){
                    strcpy(md5,xmlNodeGetContent(tepNode->xmlChildrenNode));
                }
                
                curNode = curNode->next;
            }
            curNode = tepNode;
            
            xmlFree(szKey);
        }
        
        curNode = curNode->next;

    }
	
	FILE *inFile = fopen (md5, "r");
  	if(inFile == NULL) 
    { 
        printf("File:%s Not Found\n", md5); 
        write(newSocket,"0xFF",sizeof("0xFF"));
        return;
    } 
    write(newSocket,"0x07",sizeof("0x07"));
    sleep(5);
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


    memset(buffer,0,sizeof(buffer));
    strncpy(buffer,a,4);
    printf("%s\n",buffer);
    int SendSize = 0;
    if(write(newSocket,buffer,4) < 0)
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
        SendSize = send(newSocket,data + SendSize,num,0);
        if(SendSize == -1){
            perror("Send File Content Failed:"); 
            exit(1);
        }
        num = num-SendSize;
    }
    free(data);
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
int updatexml(char filename[],const char* md5,const char *data){
	xmlDocPtr doc;           //定义解析文件指针
    xmlNodePtr curNode;      //定义结点指针
    xmlChar *szKey;          //临时字符串变量
    char *szDocName;
    int flag = 0;
    
    doc = xmlReadFile("repository.xml","GB2312",XML_PARSE_RECOVER);
    
    if (NULL == doc) {
        fprintf(stderr,"Document not parsed successfully.");
        return -1;
    }
    //获取根节点
    curNode = xmlDocGetRootElement(doc);
    if (NULL == curNode) {
        fprintf(stderr,"empty document");
        xmlFreeDoc(doc);
        return -1;
    }

    curNode = curNode->xmlChildrenNode;
    xmlNodePtr propNodePtr = curNode;
    while(curNode != NULL) {
    	printf("curNode is: %s\n",xmlNodeGetContent(curNode));
        //取出节点中的内容
        if ((!xmlStrcmp(curNode->name, (const xmlChar *) "file"))) {
            szKey = xmlNodeGetContent(curNode->xmlChildrenNode);
            printf("newNode1: %s\n", szKey);
            if(strcmp(szKey,md5) == 0){
            	printf("append %s to %s",md5,szKey);
            	xmlNewTextChild(curNode, NULL, BAD_CAST "knownas", BAD_CAST filename);
            	flag = 1;
            	break;
            }
            

            xmlFree(szKey);
        }
        curNode = curNode->next;

    }
    
    if(flag == 0){
    	curNode = xmlDocGetRootElement(doc);
    	printf("add new node\n");
    	xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "file");
    	xmlAddChild(curNode,node);
    	xmlNewTextChild(node, NULL, BAD_CAST "hashname", BAD_CAST md5);
    	xmlNewTextChild(node, NULL, BAD_CAST "knownas", BAD_CAST filename);
    	FILE *fp = fopen(md5, "ab");
    	if (fp != NULL)
    	{
        	fputs(data, fp);
        	fclose(fp);
    	}
    }
    
    int nRel = xmlSaveFile("repository.xml",doc);

    //释放文档内节点动态申请的内存
    xmlFreeDoc(doc);
	return 1;
}
