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

#define FILE_SIZE 500 
#define BUFFER_SIZE 1024

int new_fd = 0; 
int serverSocket;
Squeue *map = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t lock_cond = PTHREAD_COND_INITIALIZER;
int G_MUTEX = 1;
int map_inuse=0;
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
	remove(".dedup");							//remove the original file
	
	xmlDocPtr doc;								//opean a new xml file
	xmlNodePtr curNode;      
	xmlChar *szKey;          
	char *szDocName;

	xmlChar *result	= NULL;							//set a xml pointer
	doc = xmlNewDoc(BAD_CAST "1.0");  

	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST "repository");		//set the xml root
	xmlDocSetRootElement(doc,root_node);    

	int flag = 0;
	xmlNodePtr tepNode; 	
	   
	while((map)->first!=NULL)						//free the memory til the squeue is empty
	{
		printf("add new node\n");
		curNode = xmlDocGetRootElement(doc);				//get the root node
		curNode = curNode->xmlChildrenNode;				//get the first node
				
		while(curNode != NULL) {					//for each node, check the md5

			if ((!xmlStrcmp(curNode->name, (const xmlChar *) "file"))) {
            
				szKey = xmlNodeGetContent(curNode->xmlChildrenNode);//if current md5 is alredy exist in the xml file
				if(strcmp(szKey,(map->first)->md5) == 0){
	    				xmlNewTextChild(curNode, NULL, BAD_CAST "knownas", BAD_CAST (map->first)->filename);//add the filename as knownas
					flag = 1;            
				}
            

				xmlFree(szKey);
			}
        
			curNode = curNode->next;

		}
		
		
		
		if(flag	== 0){							//else add a new node with "hashname" and "knownas"
			xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "file");
			xmlAddChild(root_node,node);
			xmlNewTextChild(node, NULL, BAD_CAST "hashname", BAD_CAST (map->first)->md5);
			xmlNewTextChild(node, NULL, BAD_CAST "knownas", BAD_CAST (map->first)->filename);
		}
		
		flag = 0;


		leaveFront(map);
	}
	int nRel = xmlSaveFile("repository.xml",doc);				//save the xml file
	rename("repository.xml",".dedup");					//rename the xml to .dedup
	xmlFreeDoc(doc);							//free the doc pointer
   	pthread_mutex_destroy(&lock);						//destory the mutex lock
   	pthread_cond_destroy(&lock_cond);					//destory the condition variables
   	close(serverSocket);							//close the socket
   	exit(1);								//exit the file
}

int main(int argc, char *argv[])
{
	//printf("In server\n");
	if(argc < 3 || argc > 3){						//check the argument number
	    printf("wrong argument number");
	    exit(1);
	}
	char *directory = argv[1];	
	int portnum = atoi(argv[2]);
	
	struct stat st = {0};							//check the directory, if it already exist,just leave it else creat it
	int ret = stat(argv[1], &st);
	printf("%d\n",ret);
	if (ret == -1) {
		mkdir(argv[1], 0700);
	}
	
	
	
	pid_t process_id = 0;							//deamon server
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
	chdir(directory);							
	// Close stdin. stdout and stderr
	//close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	
	sleep(1);
	
	struct sigaction action;						//handle the pkill
	memset(&action, 0, sizeof(action));
	action.sa_handler = term;
	sigaction(SIGTERM, &action, NULL);
     
	initSqueue(&map);
	int checker = access(".dedup",0);					//check the folder has a backup file
	
	printf("%d\n",checker);
	if (checker == 0){							//if it has, read data from the backup file
		//printf("find file");
		readxml();	
	}
	sleep(1);
	printf("not find");
	print(map,'f','r');
	
	logfile = fopen("x.log","a+");
	
	int newSocket;								//create a new socket
	struct sockaddr_storage serverStorgae;
	socklen_t addr_size;
	serverSocket=socket(AF_INET,SOCK_STREAM,0);
	
	if(-1==serverSocket)
	{
		perror("socket");
		exit(1);
	}
	
	struct sockaddr_in server_addr;
	server_addr.sin_family=AF_INET;						//IPv4
	server_addr.sin_port=htons(portnum);					//set prot
	(server_addr.sin_addr).s_addr=inet_addr("127.0.0.1");			//IP address
	memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);
	int on=1;
	if((setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)
	{
		perror("setsockopt failed");
		exit(EXIT_FAILURE);
	}

	if(-1==bind(serverSocket,(struct sockaddr *)&server_addr,sizeof(server_addr)))
	{
		perror("bind");
		exit(6);
	}
	if(-1==listen(serverSocket,50))						//the server can accept at most 50 clients
	{
		perror("listen");
		exit(7);
	}
	
	
	pthread_t tid[50];							//the server can hold at most 50 client
	int i = 0;
	int arrayInteger[50] = {0};						//the socket for each client
	//pthread_mutex_init(&lock,NULL);
	//pthread_cond_init(
	while(1)
	{

	
		addr_size = sizeof(serverStorgae);
		arrayInteger[i] = accept(serverSocket,(struct sockaddr *) &serverStorgae,&addr_size);//accept a new client
		
		if(-1==newSocket)
		{
			perror("accept");
			exit(1);       
		}
		
		printf("new_fd=%d\n",newSocket);
		

		int ret = pthread_create(&tid[i],NULL,net_thread,&arrayInteger[i]);//create a new thread for the client
		if(ret != 0){
			printf("Failed to create thread\n");
		}
		i++;
		if(i>=50){							//reset all threads
			i=0;
			while(i<1){
				pthread_join(tid[i],NULL);
				arrayInteger[i]=0;
				i+=1;
			}
			i=0;
		}
		
	}
	pthread_mutex_destroy(&lock);						//destory the mutex lock
	close(serverSocket);
	return 0;
 
}
 
 
 
void *net_thread(void * fd)
{
	//pthread_mutex_lock(&lock);
	pthread_detach(pthread_self());						//detach the thread
	
	int newSocket=*((int *)fd);
	
	printf("%d\n",newSocket);
	
	char buffer[FILE_SIZE];
	while(1)
	{
        
		memset( buffer,'\0', sizeof(buffer) );		
		if(read(newSocket, buffer, sizeof(buffer)) < 0)			//read the command from client
		{ 
			perror("Server Recieve Data Failed:"); 
			break; 
		} 
		

		char temp_buffer[5];						//get the flag of "0x0?"
		memset( temp_buffer,'\0', sizeof(temp_buffer) );	
		strncpy(temp_buffer,buffer,4);
        
        
		if(strcmp(temp_buffer,"0x02") == 0){				//if user want to update a file
			if (G_MUTEX)						//lock the queue
				pthread_mutex_lock(&lock);
			while(map_inuse	== 1){					//if other is using the map wait
				pthread_cond_wait(&lock_cond,&lock);
			}
			map_inuse = 1;
			printf("ready update\n");
			strncpy(temp_buffer,buffer+4,499-4);			//handle the update
			printf("%s\n",temp_buffer);
			update(fd,temp_buffer);
			map_inuse = 0;
			if (G_MUTEX)						//unlock the queue
    				pthread_mutex_unlock(&lock);
			memset( buffer,'\0', sizeof(buffer) );			//empty the buffer
			continue;
		}
		if(strcmp(temp_buffer,"0x06") == 0){				//if the user want to download a file
			if (G_MUTEX)
				pthread_mutex_lock(&lock);
			while(map_inuse == 1){
				pthread_cond_wait(&lock_cond,&lock);
			}
			map_inuse = 1;
			
			printf("ready download\n");
			strncpy(temp_buffer,buffer+4,499-4);
			printf("%s\n",temp_buffer);
			download(fd,temp_buffer);
        	
			map_inuse = 0;
			if (G_MUTEX)
    				pthread_mutex_unlock(&lock);
    			memset( buffer,'\0', sizeof(buffer) );
    			continue;
		}
		if(strcmp(temp_buffer,"0x00") == 0){				//if the user want to list a file
			if (G_MUTEX)
				pthread_mutex_lock(&lock);
			while(map_inuse == 1){
				pthread_cond_wait(&lock_cond,&lock);
			}
			map_inuse = 1;
			
			printf("ready list\n");
			list(fd);
        	
			map_inuse = 0;
			if (G_MUTEX)
    				pthread_mutex_unlock(&lock);
			memset( buffer,'\0', sizeof(buffer) );
    			continue;
		}
		if(strcmp(temp_buffer,"0x04") == 0){				//if the user want to delet a file
			if (G_MUTEX)
				pthread_mutex_lock(&lock);
			while(map_inuse == 1){
				pthread_cond_wait(&lock_cond,&lock);
			}
			map_inuse = 1;
			
			printf("ready delete\n");
			strncpy(temp_buffer,buffer+4,499-4);
			printf("%s\n",temp_buffer);
			delete(fd,temp_buffer);
            
			map_inuse = 0;
			if (G_MUTEX)
    				pthread_mutex_unlock(&lock);
    			memset( buffer,'\0', sizeof(buffer) );
    			continue;
		}
		if(strcmp(temp_buffer,"0x08") == 0){				//if the user want to quit
			
			send(newSocket,"0x09",sizeof("0x09"),0);		//send a confirm to client
			memset( buffer,'\0', sizeof(buffer) );
			break;							//break the loop, disconnect
		}
		memset( buffer,'\0', sizeof(buffer) );
        
     
		
        
	}

	close(newSocket);
	
}
void delete(void * fd, char input[]){
	int newSocket=*((int *)fd);						//get the socket
	printf("%d\n",newSocket);
	int flag = 0;								//indicate the file is found or not
	Squeue *new=malloc(sizeof(map));					//get a new queue
	new->first=NULL;
	new->last=NULL;
	char *md5;
	while((map)->first!=NULL)						//free the memory til the squeue is empty
	{									//for each node, check the filename
		
		if(strcmp((map->first)->filename,input)	== 0){			//if find the file user want to delete
			flag = 1;						//set the flag to 1
			remove((map->first)->md5);				//remove the file from the folder
			md5=malloc(strlen((map->first)->md5)+1);
			strcpy(md5,(map->first)->md5);
			leaveFront(map);					//do not add the file to new queue
		}
		else{								//other file
			addBack(new,(map->first)->filename,(map->first)->content,(map->first)->md5);
			leaveFront(map);
		}
		
	}
	while((new)->first!=NULL)						//store all node form temp squeue into orignal squeue
	{
		if(strcmp((new->first)->md5, md5) == 0){
			leaveFront(new);
			continue;
		}
		else{
			addBack(map,(new->first)->filename,(new->first)->content,(new->first)->md5);
			leaveFront(new);
		}
	}
	free(new);								//free the memory

	if(flag	== 0){								//if not find the file, send a SERROR
		send(newSocket,"0xFF",sizeof("0xFF"),0);
		return;
	}
	else{									//otherwise, send a confirm
		send(newSocket,"0x05",sizeof("0x05"),0);
		return;
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
	while((map)->first!=NULL)						//free the memort til the squeue is empty
	{
		addBack(new,(map->first)->filename,(map->first)->content,(map->first)->md5);
		leaveFront(map);
		counter	+= 1;							//go through the queue, get the total number of file
	}
	char filename[counter][BUFFER_SIZE];
	while((new)->first!=NULL)						//store all node form temp squeue into orignal squeue
	{
		addBack(map,(new->first)->filename,(new->first)->content,(new->first)->md5);
		strcpy(filename[i],(new->first)->filename);
		i += 1;
		leaveFront(new);
	}
	free(new);

	unsigned int x = counter;						//send the N to the client where N is the number of file
	unsigned char a[2];
	a[1] = (x>>8) &	0xFF;							//convert N to 2bytes
	a[0] = x & 0xFF;
	if(send(newSocket,"0x01",sizeof("0x01"),0) < 0){			//send the flag to client
		return;
	}
	if(send(newSocket,a,2,0) < 0)						//send the N
	{
		perror("Send list size Failed:"); 
		exit(1);
	}
	i = 0;
	while(counter>0){							//send each filename
        //printf("%s\n",name[counter]);
		send(newSocket,filename[i],sizeof(filename[i]),0);
		i += 1;
		counter -= 1;
	}
	return;
}
void update(void * fd,char input[]){
	int newSocket=*((int *)fd);
	char buffer[BUFFER_SIZE];
	memset(buffer,'\0',sizeof(buffer));
	
	printf("%d\n",newSocket);
	int flag = 0;								//indicate the file is already exist in queue or not
	Squeue *new=malloc(sizeof(map));
	new->first=NULL;
	new->last=NULL;
	while((map)->first!=NULL)						//free the memort til the squeue is empty
	{
		addBack(new,(map->first)->filename,(map->first)->content,(map->first)->md5);
		if(strcmp((map->first)->filename,input)	== 0){			//if queue already contain a file has same name
			flag = 1;						//set flag to 1
		}
		leaveFront(map);
	}
	while((new)->first!=NULL)					//store all node form temp squeue into orignal squeue
	{
		addBack(map,(new->first)->filename,(new->first)->content,(new->first)->md5);
		leaveFront(new);
	}
	free(new);

	
	unsigned char size[4];							//else recieve the size of file
	int x;
	unsigned int length;
	memset(size,'\0',sizeof(size));
	while((length = read(newSocket, size, sizeof(size))) > 0) 
	{
		
		x = *(int *)size;						//convert 4bytes to int
	 	if(x!=0){
	 		break;
	 	}
	 	memset(size,'\0', sizeof(size) );
	}
	printf("%d\n",x);
	char *data;
	data = (char *)malloc(x);						//malloc enough space
	memset(data,'\0',x);
	int RecvSize=0;

	while(x>0) 
	{ 
    		RecvSize = recv(newSocket,data+RecvSize,x,0);			//read data from client
    		if(RecvSize == -1){
    			perror("Recieve File Content Failed:"); 
			send(newSocket,"0xFF",sizeof("0xFF"),0);
			exit(1);
    		}

    		x = x- RecvSize;
	}
	printf("%s\n",data);

	if(flag	== 1){								//if file alredy exist
    		send(newSocket,"0xFF",sizeof("0xFF"),0);			//return a SERROR
    		fprintf(logfile,"duplicate file!\n");			
    		return;
	}
	else{
		char *md5 = convertMD5(data,strlen(data));				//get the md5
		char *filename;								//get the filename
		filename = malloc(strlen(input)+1);
		strcpy(filename,input);
		addBack(map,input,data,md5);						//add the data to squeue
	
		remove(md5);								//makesure will not write data to an exist file
		FILE *fp = fopen(md5, "ab");						//create a file and write the content
		if (fp != NULL)
		{
			fputs(data, fp);
			fclose(fp);
		}
		
	    
		free(md5);								//free memory
		free(data);
		free(filename);
		send(newSocket,"0x03",sizeof("0x03"),0);
	}


	
	
	//printf("update end\n");
	//send(newSocket,"0x03",sizeof("0x03"),0);				//send a confirm to client
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
	while((map)->first!=NULL)						//free the memort til the squeue is empty
	{
		addBack(new,(map->first)->filename,(map->first)->content,(map->first)->md5);
		if(strcmp((map->first)->filename,input)	== 0){			//if find the same filename
			flag = 1;
			filename = malloc(1+strlen((map->first)->filename));
			content = malloc(1+strlen((map->first)->content));
			md5 = malloc(1+strlen((map->first)->md5));
			
			memset(filename,'\0',strlen((map->first)->filename));	
			memset(content,'\0',strlen((map->first)->content));
			memset(md5,'\0',strlen((map->first)->md5));
			
			strcpy(filename,(map->first)->filename);		//get the content and filename form squeue
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

	if(flag	== 1){								//if we find that file, send a flag tell client ready to download
		send(newSocket,"0x07",sizeof("0x07"),0);
	}	
	else{									//else send a serror
		send(newSocket,"0xFF",sizeof("0xFF"),0);
		return;
	}

	unsigned int num = strlen(content)+1;					//get the length of the string and convert it to 4bytes
	unsigned int x;
	unsigned char a[4];

	a[3] = (num>>24) & 0xFF;
	a[2] = (num>>16) & 0xFF;
	a[1] = (num>>8) & 0xFF;
	a[0] = num & 0xFF;
	printf("%x %x %x %x \n",a[3],a[2],a[1],a[0]);
	x = *(int *)a;
	printf("%d\n", x);


	memset(buffer,'\0',sizeof(buffer));					//send the size
	strncpy(buffer,a,4);
	printf("%s\n",buffer);
	int SendSize = 0;
	if(send(newSocket,buffer,4,0) < 0)
	{
		perror("Send File size Failed:"); 
		exit(1);
	}
    
	while (num>0){								//send the content
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
char *convertMD5(const char *str,int length){					//convert a string to md5
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
int updatexml(char filename[],const char* md5){					//not use anymore
    xmlDocPtr doc;           
    xmlNodePtr curNode;      
    xmlChar *szKey;          
    char *szDocName;
    
    xmlChar *result = NULL;
    doc = xmlNewDoc(BAD_CAST "1.0");  
 
    xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST "repository");    
    xmlDocSetRootElement(doc,root_node);        
   
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
void readxml(){									//read data from a xml
    xmlDocPtr doc;           
    xmlNodePtr curNode;      
    xmlChar *szKey;          
    
    rename(".dedup","repository.xml");						//rename the .dedup to a xml file
    doc = xmlReadFile("repository.xml","GB2312",XML_PARSE_RECOVER);
    printf("readxml\n");
    if (NULL == doc) {
        fprintf(stderr,"Document not parsed successfully.");
        return;
    }
    
    curNode = xmlDocGetRootElement(doc);					//get the root node of that xml
    if (NULL == curNode) {
        fprintf(stderr,"empty document");
        xmlFreeDoc(doc);
        return;
    }
    curNode = curNode->xmlChildrenNode;
    xmlNodePtr propNodePtr = curNode;
    xmlNodePtr tepNode;      
    
    char *filename;
    char *buffer;
    char *md5;
    int length;
    while(curNode != NULL) {							//for each node
        if ((!xmlStrcmp(curNode->name, (const xmlChar *) "file"))) {
            
            tepNode = curNode;
            curNode = curNode->children->next;
	    
	    FILE * f = fopen (xmlNodeGetContent(tepNode->xmlChildrenNode), "r");//get the md5, and open the md5 file
	    printf("%s",xmlNodeGetContent(tepNode->xmlChildrenNode));
	    
	    md5 = (char *)malloc(strlen(xmlNodeGetContent(tepNode->xmlChildrenNode))+1);
	    strcpy(md5,xmlNodeGetContent(tepNode->xmlChildrenNode));
	    
	    printf("file open success\n");
	    fseek (f, 0L, SEEK_END);						//get the size of the file
	    length = ftell (f);
            fseek (f, 0L, SEEK_SET);
	    buffer = (char *)malloc((length+1)*sizeof(char));			//read the content
	    memset(buffer,'\0',length);
	    fread (buffer, 1, length, f);
            buffer[length] = '\0';						//fread will not read null terminate, so add one
            fclose (f);
	    //memset(buffer,1,length);
            while(curNode !=NULL){
                szKey = xmlNodeGetContent(curNode);
                filename = (char *)malloc(strlen(szKey)+1);			//for each filename
		if(strcmp(filename,md5) !=0){
			strcpy(filename,szKey);
		}
		else{
			free(filename);
			curNode = curNode->next;
			break;
		}
		printf("filename: %s\n",filename);
		
		
		printf("md5 is: %s\n\n",md5);
                
                addBack(map,filename,buffer,md5);				//add the filename, content, md5 as a new node 
                
		free(filename);
                curNode = curNode->next;					//go to the next filename
            }
	    free(md5);								//free the memory
	    free(buffer);
            curNode = tepNode;
	    printf("\n\n\n\n");
            xmlFree(szKey);
        }
        
        curNode = curNode->next;						//go the the next ,d5

    }
    int nRel = xmlSaveFile("repository.xml",doc);				//store the xml file
    xmlFreeDoc(doc);
    rename("repository.xml",".dedup");						//rename it as a .dedup
}