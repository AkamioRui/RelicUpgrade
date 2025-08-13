#ifndef relic_STATE
#define relic_STATE

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "relic_JSON.h"


typedef struct STATE{
    double price;//the average price
    double successChance;
    char msg[16];
    char whitelisted;//1 if true

    void *arg;//for printing
    struct STATE *child;//the address in root array where this.children start
    int childCount;

    //for processing
    struct STATE *parent;//what actually being used
    int parentCount;
    double *parentChance;//NAN = rejected, neg = accepted, pos = undecided

} STATE ;
STATE *STATE_getChild(STATE *node, int index);
int STATE_getChildCount(STATE *node);
void STATE_fprintNodeD(FILE *outFile,STATE *node);
void STATE_fprintLinkD(FILE *outFile, STATE *parentNode, STATE *childNode);
void STATE_clearArg(STATE *root);

//for computation
int moreEfficient(STATE *a,STATE *b){
    STATE *ap = (STATE *)a;
    STATE *bp = (STATE *)b;
    return (ap->successChance/ap->price) < (bp->successChance/bp->price);
}

//for global graph
typedef struct STATE_array{
    STATE *ptr;
    int len;
} STATE_array;
struct {
    STATE_array root;//len is the entire nodes

    //the following are just for references, it doesnt allocate memory
    struct{
        STATE_array start;
        STATE_array combination;
        STATE_array good;
        STATE_array upgrade[4];//index = number of good substat -1 
    } three;

    struct{
        STATE_array start;
        STATE_array good;
        STATE_array upgrade[4];
    } four;
} graph;
void graph_close();
void graph_init_test();

//----------------------------------------------------------------------------------------------------------


//for global graph
void graph_close(){
    for(int i = 0 ; i < graph.root.len ; i++){
        STATE *currentNode = graph.root.ptr+i;
        if(currentNode->arg){
            free(currentNode->arg);
            currentNode->arg = NULL;
        }
        if(currentNode->parentChance){
            free(currentNode->parentChance);
            currentNode->parentChance = NULL;
        }
        
    }
   
    
    free(graph.root.ptr);
    graph.root.ptr = NULL;

    printf("graph closed\n");
}
/* this is still for testing, only filling in the root */
void graph_init_test(){
    printf("graph initialized\n");
    // currently only for the root
    graph.root.len = 6;
    graph.root.ptr = (STATE *)calloc(graph.root.len,sizeof(STATE));
    
    for(int i = 0 ; i<6; i++){
        graph.root.ptr[i].successChance = i;
        graph.root.ptr[i].price = i*100+10;
        sprintf(graph.root.ptr[i].msg,"%c",'a'+i);
        
    }

    //child 
    int childId[] =    {1,3,4};
    int childCount[] = {2,3,1};
    for(int i = 0; i<3; i++){
        graph.root.ptr[i].childCount = childCount[i];
        graph.root.ptr[i].child = graph.root.ptr + childId[i];
    }

    //parent
    int parentId[] =    {0,0,0,1,1,1};
    int parentCount[] = {0,1,1,1,2,1};
    for(int nodeId = 0; nodeId<graph.root.len; nodeId++){
        STATE *root = graph.root.ptr;
        STATE *theNode = root+nodeId;
        theNode->parentCount = parentCount[nodeId];
        theNode->parent = root + parentId[nodeId];
        theNode->parentChance = (double*)malloc(sizeof(double)*parentCount[nodeId]);
        for(int parentIdx = 0; parentIdx<theNode->parentCount; parentIdx++){
            theNode->parentChance[parentIdx] = nodeId*100 + theNode->parent+parentIdx-root;    
        }
    }
    
  
    atexit(graph_close);
}

STATE *STATE_getChild(STATE *node, int index){return node->child + index;}
int STATE_getChildCount(STATE *node){return node->childCount;}
void STATE_fprintNodeD(FILE *outFile,STATE *node){
    
    fprintf(outFile,"{");
    fprintf(outFile," \"detail\":\"%s\"",node->msg);
    fprintf(outFile,",\"price\":%.2lf"  ,node->price);
    fprintf(outFile,",\"succesR\":%.2lf",node->successChance);
    fprintf(outFile,",\"accept\":%d"    ,node->whitelisted);
    fprintf(outFile,"}");
}
void STATE_fprintLinkD(FILE *outFile, STATE *parentNode, STATE *childNode){
    fprintf(
        outFile,
        "{\"chance\":%.2lf}"
        ,childNode->parentChance[parentNode - childNode->parent]
    );
}
void STATE_clearArg(STATE *root){
    for(int i = 0 ; i < graph.root.len ; i++){
        STATE *currentNode = graph.root.ptr+i;
        if(currentNode->arg){
            free(currentNode->arg);
            currentNode->arg = NULL;
        }
    }
}


__json_printSpannigTree_generic(STATE)


//drawing a STATE graph
// graph_init_test();
// JSON *mainGraphJson = json_init("mydata.json");
// json_printGraph(
//     mainGraphJson,
//     (void *)graph.root.ptr,
//     (void (*)(void *, FILE *, FILE *, int *, int *))__json_printSpanningTree_STATE
// );
// json_close(mainGraphJson);

#endif