#include<stdio.h>
#include<stdlib.h>
#include<string.h>


// global variable
typedef enum PIECE{
    PIECE_HEAD,
    PIECE_HANDS,
    PIECE_BODY,
    PIECE_FEET,
    PIECE_PLANAR,
    PIECE_ROPE
} PIECE;
typedef enum STAT{
    STAT_HP = 0,
    STAT_ATK = 1,
    STAT_DEF = 2,
    STAT_HPP = 3,
    STAT_ATKP = 4,
    STAT_DEFP = 5,
    STAT_SPD = 6,
    STAT_CR = 7,
    STAT_CD = 8,
    STAT_EHR = 9,
    STAT_ERS = 10,
    STAT_BE = 11,
    STAT_DMG = -1,
    STAT_HEAL = -2,
    STAT_ER = -3
} STAT;

struct COST{
    double create;
    double upgrade[5];
    double accept;
} cost;
struct CHANCE{
    double mainstat;
    int substatWeightTotal;
    int substatWeight[12];
    int substatGood[12];//1 if good, 0 if bad
} chance;
/* done */void iniGlobalVariable(PIECE piece, STAT mainstat, STAT *substat, int substat_len);

//for STATE
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
void STATE_clearArg();


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


//for min HEAP
typedef struct HEAP_STATE{
    int length;
    STATE *data;
    struct HEAP_STATE *left;
    struct HEAP_STATE *right;
} HEAP_STATE;
HEAP_STATE* HEAP_STATE_init(STATE * data);
void HEAP_STATE_free(HEAP_STATE * root);
HEAP_STATE* HEAP_STATE_at(HEAP_STATE * root, int index);
void HEAP_STATE_add(HEAP_STATE * root, STATE * data);
HEAP_STATE* HEAP_STATE_pop(HEAP_STATE * root);


HEAP_STATE* HEAP_STATE_init(STATE * data){
    HEAP_STATE *result = calloc(1,sizeof(HEAP_STATE));
    result->length = 1;
    result->data = data;
    return result;
}
HEAP_STATE* HEAP_STATE_at(HEAP_STATE * root, int index){
    if(index < 0)return NULL;
    if(index == 0) return root;
    if(index >= root->length)return NULL;
    index += 1;//root start at index 1;

    //get depth
    int depth = 0;
    for(int _index = index; _index>0; _index >> 1)depth++;
    
    //for all level(excluding the highest order), get 0/1, move pointer to
    HEAP_STATE *result = root;
    for(int level = depth -1, code; level >= 0; level--){
        code = (index >> (level - 1)) / 2;
        result = code ? (result->right) : (result->left);
    }

    
    
}



//for JSON
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
__json_printSpannigTree_generic(STATE)








int main(){
    // // initialize constant variable
    // STAT substat[] = {STAT_CD,STAT_ATKP};
    // iniGlobalVariable(PIECE_BODY,STAT_CR,substat,sizeof(substat)/sizeof(int)); 
    // struct COST cost2;
    // struct CHANCE chance2;
    // memcpy(&cost2,&cost,sizeof(struct COST));
    // memcpy(&chance2,&chance,sizeof(struct CHANCE));

    
    graph_init_test();



    JSON *mainGraphJson = json_init("mydata.json");
    json_printGraph(
        mainGraphJson,
        (void *)graph.root.ptr,
        (void (*)(void *, FILE *, FILE *, int *, int *))__json_printSpanningTree_STATE
    );
    
    json_close(mainGraphJson);
    
    

    

    
    printf("\ndone\n");
    return 0;
}
void iniGlobalVariable(PIECE piece, STAT mainstat, STAT *substat, int substat_len){
    //cost
        //initial
        cost.create = 100*195;// 1 remain is 195 exp
        cost.upgrade[0] = 2400;//from lvl 0 to 3 need # exp
        cost.upgrade[1] = 5100;//from lvl 3 to 6need # exp
        cost.upgrade[2] = 10000;
        cost.upgrade[3] = 19500;
        cost.upgrade[4] = 39000;
    
        //refund 
        cost.create -= 1500;
        cost.accept = 1500;//all sum of refund
        for(int i = 0; i<5; i++){
            cost.upgrade[i] /= 5;//refunded 80%
            cost.accept += cost.upgrade[i]*4;
        } 
    //

    //chance.mainstat
    switch (piece){
        case PIECE_HEAD:
            chance.mainstat = 1;
        break;
        case PIECE_HANDS:
            chance.mainstat = 1;
        break;
        case PIECE_BODY:
            switch (mainstat){
                case STAT_HPP:chance.mainstat = 0.20f;break;
                case STAT_ATKP:chance.mainstat = 0.20f;break;
                case STAT_DEFP:chance.mainstat = 0.20f;break;
                case STAT_CD:chance.mainstat = 0.10f;break;
                case STAT_CR:chance.mainstat = 0.10f;break;
                case STAT_EHR:chance.mainstat = 0.10f;break;
                case STAT_HEAL:chance.mainstat = 0.10f;break;
                default :chance.mainstat = 0;break;
            }
        break;
        case PIECE_FEET:
            switch (mainstat){
                case STAT_HPP:chance.mainstat = 0.28f;break;
                case STAT_ATKP:chance.mainstat = 0.30f;break;
                case STAT_DEFP:chance.mainstat = 0.30f;break;
                case STAT_SPD:chance.mainstat = 0.12f;break;
                default :chance.mainstat = 0;break;
            }
        break;
        case PIECE_PLANAR:
            switch (mainstat){
                case STAT_HPP:chance.mainstat = 0.12f;break;
                case STAT_ATKP:chance.mainstat = 0.13f;break;
                case STAT_DEFP:chance.mainstat = 0.12f;break;
                case STAT_DMG:chance.mainstat = 0.09f;break;
                default :chance.mainstat = 0;break;
            }
        break;
        case PIECE_ROPE:
            switch (mainstat){
                case STAT_HPP:chance.mainstat = 0.26f;break;
                case STAT_ATKP:chance.mainstat = 0.27f;break;
                case STAT_DEFP:chance.mainstat = 0.24f;break;
                case STAT_BE:chance.mainstat = 0.16f;break;
                case STAT_ER:chance.mainstat = 0.5f;break;
                default :chance.mainstat = 0;break;
            }
        break;
        default: printf("invalid piece"); break;
    }
    //chance.substatWeight
    int substatWeight[] = {10,10,10,10,10,10,4,6,6,8,8,8};
    if(mainstat >= 0)substatWeight[mainstat] = 0;
    memcpy(chance.substatWeight,substatWeight,12*sizeof(int));
    
    //chance.substatweightTotal 
    chance.substatWeightTotal = 0;
    for(int i = 0; i<sizeof(substatWeight)/sizeof(int); i++){
        chance.substatWeightTotal += substatWeight[i];
    }

    //chance.substatGood
    memset(chance.substatGood,0,sizeof(chance.substatGood));
    for(int i = 0; i < substat_len; i++){
        chance.substatGood[substat[i]] = 1;
    }
    
    
    
    
}


//for STATE
/* 
return the pointer to the child
*/
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
void STATE_clearArg(){
    for(int i = 0 ; i < graph.root.len ; i++){
        STATE *currentNode = graph.root.ptr+i;
        if(currentNode->arg){
            free(currentNode->arg);
            currentNode->arg = NULL;
        }
    }
}


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





//for JSON
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
    print into treeF: {treeLinkObject},[] 
    print into outerF: {outerLinkObject},[]
    return _nodeCount and _outerExist
*/
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
                if(queueTail)  queueTail = queueTail->next = malloc(sizeof(QUEUE));
                else  queueTail = queueRoot = malloc(sizeof(QUEUE));
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
    NODE_clearArg();
    #undef NODE
    #undef NODE_getChild
    #undef NODE_getChildCount
    #undef NODE_fprintNodeD
    #undef NODE_fprintLinkD
    #undef NODE_clearArg
}

