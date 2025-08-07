
typedef struct{
    void *arg;
}STATE;
STATE *STATE_getChild(STATE *node, int index);
int   STATE_getChildCount(STATE *node);
void  STATE_fprintNodeD(FILE *outFile,STATE *node);
void  STATE_fprintLinkD(FILE *outFile, STATE *parentNode, STATE *childNode);
void  STATE_clearArg();

/* 
TYPE{
void* arg;
have all function
}
*/

#define __json_printSpannigTree_generic(TYPE) \
void __json_printSpanningTree_##TYPE##(void* _root, FILE *treeF, FILE *outerF, int *nodeCount, int *outer_exist){\
    \
    *outer_exist = 0;\
    *nodeCount = 0;\
    \
    TYPE *root = (TYPE *)_root;\
    typedef struct QUEUE {\
        TYPE *node;\
        struct QUEUE *next;\
    } QUEUE;\
    QUEUE *queueTemp = NULL, *queueRoot = NULL, *queueTail = NULL;\
    TYPE *theNode;\
    \
    root->arg = malloc(sizeof(int));\
    *(int *)root->arg = (*nodeCount)++;\
    fprintf(treeF,"            {");\
    fprintf(treeF," \"id\":%d",*(int *)root->arg);\
    fprintf(treeF,",\"parentId\":\"\"");\
    fprintf(treeF,",\"linkData\":{\"chance\":100}");\
    fprintf(treeF,",\"nodeData\":"); TYPE##_fprintNodeD(treeF,root);\
    fprintf(treeF,"},\n");\
    \
    theNode = root;\
    while(1){\
        int theNodeChildCount = TYPE##_getChildCount(theNode);\
        for(int i = 0; i < theNodeChildCount; i++){\
            TYPE *child = TYPE##_getChild(theNode,i);\
            if(!child->arg){\
                \
                child->arg = malloc(sizeof(int));\
                *(int *)child->arg = (*nodeCount)++;\
\
                \
                if(queueTail)  queueTail = queueTail->next = malloc(sizeof(QUEUE));\
                else  queueTail = queueRoot = malloc(sizeof(QUEUE));\
                queueTail->node = child;\
                queueTail->next = NULL;\
                \
\
                \
                fprintf(treeF,"            {");\
                fprintf(treeF," \"id\":%d",*(int *)child->arg);\
                fprintf(treeF,",\"parentId\":%d",*(int *)theNode->arg);\
                fprintf(treeF,",\"linkData\":");\
                TYPE##_fprintLinkD(treeF,theNode,child);\
                fprintf(treeF,",\"nodeData\":");\
                TYPE##_fprintNodeD(treeF,child);\
                fprintf(treeF,"},\n");\
\
            } else {\
                *outer_exist = 1;\
                fprintf(outerF,"            {");\
                fprintf(outerF," \"id\":%d",*(int *)child->arg);\
                fprintf(outerF,",\"parentId\":%d",*(int *)theNode->arg);\
                fprintf(outerF,",\"linkData\":");\
                TYPE##_fprintLinkD(outerF,theNode,child);\
                fprintf(outerF,"},\n");\
            }\
\
            \
        }\
        \
        \
        \
        if(queueRoot){\
            theNode = queueRoot->node;\
            if(queueRoot->next){\
                queueTemp = queueRoot;\
                queueRoot = queueRoot->next;\
                free(queueTemp);\
            }else{\
                free(queueRoot);\
                queueTail = queueRoot = NULL;\
            }\
\
        } else break;\
\
    }\
\
\
    \
    TYPE##_clearArg();\
}



