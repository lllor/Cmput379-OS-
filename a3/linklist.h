#include <stdbool.h>

struct Node {
    int pagenum;
    struct Node * next;
};
typedef struct{
    struct Node* first;
    struct Node* last;
} Linklist;
bool isEmpty(const Linklist *linklist);
void initLinklist(Linklist **linklist);
void leaveFront(Linklist *linklist);
void addBack(Linklist *linklist, int pagenum);
void removeCurrent(Linklist *linklist, int pagenum);
void nuke(Linklist *linklist);
void destroy(Linklist **linklist);

