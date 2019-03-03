// createXmlFile.cpp
//complie command: gcc -o test.out -std=c99 `xml2-config --cflags` creatXml.c `xml2-config --libs`
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

 
int main(int argc, char **argv)

{

    xmlChar *result = NULL;
    int size = 0;
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");  //定义文档和节点指针
 
    xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST "root");    
    xmlDocSetRootElement(doc,root_node);        //设置根节点
    
    //在根节点中直接创建节点
    xmlNewTextChild(root_node, NULL, BAD_CAST "newNode1", BAD_CAST "newNode1 content");
    xmlNewTextChild(root_node, NULL, BAD_CAST "newNode2", BAD_CAST "newNode2 content");
    xmlNewTextChild(root_node, NULL, BAD_CAST "newNode3", BAD_CAST "newNode3 content");

    //创建一个节点，设置其内容和属性，然后加入根结点
    xmlNodePtr node    = xmlNewNode(NULL,BAD_CAST "node2");
    xmlNodePtr content = xmlNewText(BAD_CAST "NODE CONTENT");
    xmlAddChild(root_node,node);
    xmlAddChild(node,content);
    xmlNewProp(node,BAD_CAST "attribute",BAD_CAST "yes");
    
    //创建一个儿子和孙子节点
    node = xmlNewNode(NULL, BAD_CAST "son");
    xmlAddChild(root_node,node);
    xmlNodePtr grandson = xmlNewNode(NULL, BAD_CAST "grandson");
    xmlAddChild(node,grandson);

    xmlNewTextChild(grandson, NULL, BAD_CAST "newGNode1", BAD_CAST "newGNode1 content");
    //xmlAddChild(grandson, xmlNewText(BAD_CAST "This is a grandson node"));
    
    xmlNodePtr congson = xmlNewNode(NULL, BAD_CAST "congson");
    xmlAddChild(grandson,congson);

    //存储xml文档
    //xmlKeepBlanksDefault(0);
    //xmlDocDumpFormatMemoryEnc(doc, &result, &size, "UTF-8", 1);

    int nRel = xmlSaveFile("CreateXml.xml",doc);
    if (nRel != -1)
    {
        printf("一个xml文档被创建，写入%d个字节\n", nRel);
    }
    //释放文档内节点动态申请的内存
    xmlFreeDoc(doc);
    return 1;
}