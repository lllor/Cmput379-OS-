// createXmlFile.cpp
//complie command: gcc -o test.out -std=c99 `xml2-config --cflags` creatXml.c `xml2-config --libs`
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
xmlNodePtr findNodeByName(xmlNodePtr rootnode, const xmlChar * nodename,const char * filename);
int main(int argc, char **argv)

{
    xmlDocPtr doc;           //定义解析文件指针
    xmlNodePtr curNode;      //定义结点指针
    xmlChar *szKey;          //临时字符串变量
    char *szDocName;
    
    doc = xmlReadFile("copy.xml","GB2312",XML_PARSE_RECOVER);
    
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
    while(curNode != NULL) {
        //取出节点中的内容
        if ((!xmlStrcmp(curNode->name, (const xmlChar *) "file"))) {
            tepNode = curNode;
            curNode = curNode->children;
            while(curNode !=NULL){
                szKey = xmlNodeGetContent(curNode);
                //printf()
                if(strcmp(szKey,"1.txt") == 0){
                    flag = 1;
                }
                
                curNode = curNode->next;
            }
            curNode = tepNode;
            
            xmlFree(szKey);
        }
        if(flag == 1){
            tepNode = curNode -> next;
            xmlUnlinkNode(curNode);
            xmlFreeNode(curNode);
            curNode = tepNode;
            flag = 0;
            continue;
        }
        
        curNode = curNode->next;

    }
    int nRel = xmlSaveFile("copy.xml",doc);
    if (nRel != -1)
    {
        printf("一个xml文档被创建，写入%d个字节\n", nRel);
    }
    //释放文档内节点动态申请的内存
    xmlFreeDoc(doc);

//================================================================================
//查找条例
	// xmlDocPtr doc;           //定义解析文件指针
 //    xmlNodePtr curNode;      //定义结点指针
 //    xmlChar *szKey;          //临时字符串变量
    
    
 //    doc = xmlReadFile("copy.xml","GB2312",XML_PARSE_RECOVER);
 //    //解析文件
 //    //检查解析文档是否成功，如果不成功，libxml将报错并停止解析。
 //    //一个常见错误是不适当的编码，XML标准文档除了用UTF-8或UTF-16外还可用其它编码保存
 //    if (NULL == doc) {
 //        fprintf(stderr,"Document not parsed successfully.");
 //        return -1;
 //    }
 //    //获取根节点
 //    curNode = xmlDocGetRootElement(doc);
 //    if (NULL == curNode) {
 //        fprintf(stderr,"empty document");
 //        xmlFreeDoc(doc);
 //        return -1;
 //    }
 //    // xmlNodePtr temp;
 //    // temp = findNodeByName(curNode,(const xmlChar *) "knownas","1.txt");
 //    // if(temp != NULL){
 //    //     szKey = xmlNodeGetContent(temp);
 //    //     printf("%s\n",szKey);
 //    // }
 //    // else{
 //    //     printf("errpr");
 //    // }
    
 //    //确认根元素名字是否符合
 //    // if (xmlStrcmp(curNode->name, BAD_CAST "root")) {
 //    //     fprintf(stderr,"document of the wrong type, root node != root");
 //    //     xmlFreeDoc(doc);
 //    //     return -1;
 //    // }
 //    int flag;
 //    curNode = curNode->xmlChildrenNode;
 //    xmlNodePtr propNodePtr = curNode;
 //    xmlNodePtr tepNode;      //定义结点指针
 //    while(curNode != NULL) {
 //        //取出节点中的内容
 //        if ((!xmlStrcmp(curNode->name, (const xmlChar *) "file"))) {
 //            tepNode = curNode;
 //            curNode = curNode->children;
 //            while(curNode !=NULL){
 //                szKey = xmlNodeGetContent(curNode);
 //                if(strcmp(szKey,"1.txt") == 0){
 //                    printf("md5: %s\n", xmlNodeGetContent(tepNode->xmlChildrenNode));
 //                }
                
 //                curNode = curNode->next;
 //            }
 //            curNode = tepNode;
 //            // tepNode = curNode -> xmlChildrenNode;
 //            // szKey = xmlNodeGetContent(tepNode);
 //            // printf("newNode1: %s\n", szKey);
 //            //if(strcmp(szKey,"0000") == 0){
 //            //	xmlNewTextChild(curNode, NULL, BAD_CAST "knownas", BAD_CAST "1.txt");
 //            //}
            

 //            xmlFree(szKey);
 //        }
 //        //查找带有属性attribute的节点
 //        if (xmlHasProp(curNode,BAD_CAST "attribute")) {
 //            propNodePtr = curNode;
 //        }
 //        curNode = curNode->next;

 //    }
 //    int nRel = xmlSaveFile("copy.xml",doc);
 //    if (nRel != -1)
 //    {
 //        printf("一个xml文档被创建，写入%d个字节\n", nRel);
 //    }
 //    //释放文档内节点动态申请的内存
 //    xmlFreeDoc(doc);
//=================================================================================================
//添加条例
	// xmlDocPtr doc;           //定义解析文件指针
 //    xmlNodePtr curNode;      //定义结点指针
 //    xmlChar *szKey;          //临时字符串变量
 //    char *szDocName;

 //    doc = xmlReadFile("copy.xml","GB2312",XML_PARSE_RECOVER);
 //    if (NULL == doc) {
 //        fprintf(stderr,"Document not parsed successfully.");
 //        return -1;
 //    }
 //    //获取根节点
 //    curNode = xmlDocGetRootElement(doc);
 //    if (NULL == curNode) {
 //        fprintf(stderr,"empty document");
 //        xmlFreeDoc(doc);
 //        return -1;
 //    }
 //    xmlNodePtr node2 = xmlNewNode(NULL, BAD_CAST "file");
 //    xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "file");
 //    xmlAddChild(curNode,node);
 //    xmlAddChild(curNode,node2);
 //    xmlNewTextChild(node, NULL, BAD_CAST "hashname", BAD_CAST "1234455667789890");
 //    xmlNewTextChild(node2, NULL, BAD_CAST "hashname", BAD_CAST "0000");
 //    int nRel = xmlSaveFile("copy.xml",doc);
 //    if (nRel != -1)
 //    {
 //        printf("一个xml文档被创建，写入%d个字节\n", nRel);
 //    }
 //    //释放文档内节点动态申请的内存
 //    xmlFreeDoc(doc);
//======================================================================================================	
//create xml 
    // xmlChar *result = NULL;
    // int size = 0;
    // xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");  //定义文档和节点指针
 
    // xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST "root");    
    // xmlDocSetRootElement(doc,root_node);        //设置根节点
    // char name[7] = "hello";
    // //在根节点中直接创建节点
    // xmlNewTextChild(root_node, NULL, BAD_CAST "newNode1", BAD_CAST name);
    // xmlNewTextChild(root_node, NULL, BAD_CAST "newNode2", BAD_CAST "newNode2 content");
    // xmlNewTextChild(root_node, NULL, BAD_CAST "newNode3", BAD_CAST "newNode3 content");

    // //创建一个节点，设置其内容和属性，然后加入根结点
    // xmlNodePtr node    = xmlNewNode(NULL,BAD_CAST "node2");
    // xmlNodePtr content = xmlNewText(BAD_CAST "NODE CONTENT");
    // xmlAddChild(root_node,node);
    // xmlAddChild(node,content);
    // xmlNewProp(node,BAD_CAST "attribute",BAD_CAST "yes");
    
    // //创建一个儿子和孙子节点
    // node = xmlNewNode(NULL, BAD_CAST "son");
    // xmlAddChild(root_node,node);
    // xmlNodePtr grandson = xmlNewNode(NULL, BAD_CAST "grandson");
    // xmlAddChild(node,grandson);

    // xmlNewTextChild(grandson, NULL, BAD_CAST "newGNode1", BAD_CAST "newGNode1 content");
    // //xmlAddChild(grandson, xmlNewText(BAD_CAST "This is a grandson node"));
    
    // xmlNodePtr congson = xmlNewNode(NULL, BAD_CAST "congson");
    // xmlAddChild(grandson,congson);

    // //存储xml文档
    // //xmlKeepBlanksDefault(0);
    // //xmlDocDumpFormatMemoryEnc(doc, &result, &size, "UTF-8", 1);

    // int nRel = xmlSaveFile(".dedup",doc);
    // if (nRel != -1)
    // {
    //     printf("一个xml文档被创建，写入%d个字节\n", nRel);
    // }
    // //释放文档内节点动态申请的内存
    // xmlFreeDoc(doc);
    // rename(".dedup","copy.xml");
    //int ret = rename("dudup.txt",".dedup");
    //printf("%d",ret);
    return 1;
}

xmlNodePtr findNodeByName(xmlNodePtr rootnode, const xmlChar * nodename, const char * filename)
{
    xmlChar *szKey;
    xmlNodePtr node = rootnode;
    if(node == NULL){
        printf("Document is empty!");
        return NULL;
    }

    while(node != NULL){

        if(!xmlStrcmp(node->name, nodename)){
            return node; 
        }
        else if (node->children != NULL) {

            xmlNodePtr intNode =  findNodeByName(node->children, nodename, filename); 
            if(!xmlStrcmp(intNode->name, (const xmlChar *) "file")){
                return intNode;
            }
            if(intNode != NULL) {
                szKey = xmlNodeGetContent(intNode);
                
                if(strcmp(szKey,filename) == 0){
                    //printf("%s\n",szKey);
                    printf("current node is %s\n",xmlNodeGetContent(node));
                    //return node;
                }
                return intNode;
            }
        }
        node = node->next;
    }
    return NULL;
}
