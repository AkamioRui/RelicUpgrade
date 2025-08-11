#ifndef relic_JSON
#define relic_JSON

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct {
    FILE *outFile;
} JSON ;
JSON *json_init(char *filename);
void json_close(JSON *json);
void __json_printSpanningTree(void* _root, FILE *treeF, FILE *outerF, int *nodeCount, int *outer_exist);

void json_printGraph(
    JSON *json, 
    void *root, 
    void (*json_printSpanningTree)(void*, FILE *, FILE *, int *, int *)
);


//      requirement:
// 1. have void *arg in the property
// 2. have these function
// TYPE *TYPE_getChild(TYPE *node, int index);
// int TYPE_getChildCount(TYPE *node);
// void TYPE_fprintNodeD(FILE *outFile,TYPE *node);
// void TYPE_fprintLinkD(FILE *outFile, TYPE *parentNode, TYPE *childNode);
// void TYPE_clearArg(TYPE* root);
// 3. write this __json_printSpannigTree_generic(TYPE)
//
//      function:
//
// print into treeF: {treeLinkObject},[] 
// print into outerF: {outerLinkObject},[]
// return _nodeCount and _outerExist
#define __json_printSpannigTree_generic(TYPE) \
void __json_printSpanningTree_##TYPE(TYPE* root, FILE *treeF, FILE *outerF, int *nodeCount, int *outer_exist){\
    \
    *outer_exist = 0;\
    *nodeCount = 0;\
    \
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
                if(queueTail)  queueTail = queueTail->next = (QUEUE*)malloc(sizeof(QUEUE));\
                else  queueTail = queueRoot = (QUEUE*)malloc(sizeof(QUEUE));\
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
    TYPE##_clearArg(root);\
}

/* print into file: [ */
JSON *json_init(char *filename){
    JSON *json = (JSON *)calloc(1,sizeof(JSON));
    
    json->outFile = fopen(filename,"w");
    fprintf(json->outFile,"[\n");
    return json;
}
/* print into json: ] */
void json_close(JSON *json){
    FILE *outFile = json->outFile;

    if(ftell(outFile) >3)fseek(outFile,-3,SEEK_CUR);
    fprintf(outFile," \n]");
    fflush(outFile);

    fclose(outFile);
    free(json);
}
/* print into json: {"treeLink":[...],"outerLink:[...]"}, */
void json_printGraph(
    JSON *json, 
    void *root, 
    void (*json_printSpanningTree)(void*, FILE *, FILE *, int *, int *)
){
    FILE *outFile = json->outFile;
    fprintf(outFile,"    {\n");

    
    //for each object property open array (create new file for buffer)
    FILE *treeLinkFile = outFile;
    fprintf(treeLinkFile,"        \"treeLinks\":[\n");
    int availableId = 0;
    FILE *outerLinkFile = tmpfile();
    fprintf(outerLinkFile,"        \"outerLinks\":[\n");
    int _outer_exist = 0;
    
    
    json_printSpanningTree(
        root,
        treeLinkFile, 
        outerLinkFile,
        &availableId,
        &_outer_exist
    );

    
    //close both array
    if(availableId)fseek(treeLinkFile,-3,SEEK_CUR);
    fprintf(treeLinkFile ,"\n        ],\n");
    if(_outer_exist)fseek(outerLinkFile,-2,SEEK_CUR);
    fprintf(outerLinkFile,"\n        ]\n");
    
    //print outerLinkFile
    int count;
    char buffer[1024];
    for(rewind(outerLinkFile); (count = fread(buffer,1,1024,outerLinkFile)) != 0 ;){
        fwrite(buffer,1,count,treeLinkFile);
    }

    //clean up
    fclose(outerLinkFile);

    fprintf(outFile,"    },\n");
}
/* 
    
*/

/*
reserved if needed more modification 
void __json_printSpanningTree(void* _root, FILE *treeF, FILE *outerF, int *nodeCount, int *outer_exist){
    #define NODE STATE
    #define NODE_getChild STATE_getChild
    #define NODE_getChildCount STATE_getChildCount
    #define NODE_fprintNodeD STATE_fprintNodeD
    #define NODE_fprintLinkD STATE_fprintLinkD
    #define NODE_clearArg STATE_clearArg

    //result
    *outer_exist = 0;
    *nodeCount = 0;
      
    //for bfs
    NODE *root = (NODE *)_root;
    typedef struct QUEUE {
        NODE *node;
        struct QUEUE *next;
    } QUEUE;
    QUEUE *queueTemp = NULL, *queueRoot = NULL, *queueTail = NULL;
    NODE *theNode;
    
    
    //symbollically placing root on queue
    root->arg = malloc(sizeof(int));
    *(int *)root->arg = (*nodeCount)++;

    fprintf(treeF,"            {");
    fprintf(treeF," \"id\":%d",*(int *)root->arg);
    fprintf(treeF,",\"parentId\":\"\"");
    fprintf(treeF,",\"linkData\":{\"chance\":100}");
    fprintf(treeF,",\"nodeData\":");NODE_fprintNodeD(treeF,root);
    fprintf(treeF,"},\n");
    //pick root as current link
    theNode = root;

    while(1){
        int theNodeChildCount = NODE_getChildCount(theNode);
        for(int i = 0; i < theNodeChildCount; i++){
            NODE *child = NODE_getChild(theNode,i);

            if(!child->arg){//if it has never been in queue,

                //give id
                child->arg = malloc(sizeof(int));
                *(int *)child->arg = (*nodeCount)++;

                //add to queue
                if(queueTail)  queueTail = queueTail->next = (QUEUE*)malloc(sizeof(QUEUE));
                else  queueTail = queueRoot = (QUEUE*)malloc(sizeof(QUEUE));
                queueTail->node = child;
                queueTail->next = NULL;
                

                //print link(this,child) in treelink along with its data, 
                fprintf(treeF,"            {");
                fprintf(treeF," \"id\":%d",*(int *)child->arg);
                fprintf(treeF,",\"parentId\":%d",*(int *)theNode->arg);
                fprintf(treeF,",\"linkData\":");
                NODE_fprintLinkD(treeF,theNode,child);
                fprintf(treeF,",\"nodeData\":");
                NODE_fprintNodeD(treeF,child);
                fprintf(treeF,"},\n");

            } else {//else print this link in outer
                *outer_exist = 1;
                fprintf(outerF,"            {");
                fprintf(outerF," \"id\":%d",*(int *)child->arg);
                fprintf(outerF,",\"parentId\":%d",*(int *)theNode->arg);
                fprintf(outerF,",\"linkData\":");
                NODE_fprintLinkD(outerF,theNode,child);
                fprintf(outerF,"},\n");
            }

            
        }
        
        
        //pick current node
        if(queueRoot){
            theNode = queueRoot->node;
            if(queueRoot->next){
                queueTemp = queueRoot;
                queueRoot = queueRoot->next;
                free(queueTemp);
            }else{
                free(queueRoot);
                queueTail = queueRoot = NULL;
            }

        } else break;

    }


    //clean up
    NODE_clearArg(_root);
    #undef NODE
    #undef NODE_getChild
    #undef NODE_getChildCount
    #undef NODE_fprintNodeD
    #undef NODE_fprintLinkD
    #undef NODE_clearArg
}
 */


#endif