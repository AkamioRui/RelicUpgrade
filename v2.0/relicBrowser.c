#include<stdio.h>
#include<stdlib.h>
#include<string.h>


// global variable
#define PIECE_HEAD 0
#define PIECE_HANDS 1
#define PIECE_BODY 2
#define PIECE_FEET 3
#define PIECE_PLANAR 4
#define PIECE_ROPE 5

#define STAT_HP 0
#define STAT_ATK 1
#define STAT_DEF 2
#define STAT_HPP 3
#define STAT_ATKP 4
#define STAT_DEFP 5
#define STAT_SPD 6
#define STAT_CR 7
#define STAT_CD 8
#define STAT_EHR 9
#define STAT_ERS 10
#define STAT_BE 11
#define STAT_DMG -1
#define STAT_HEAL -2
#define STAT_ER -3

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


typedef struct STATE{
    double price;//the average price
    double successChance;
    char msg[16];

    
    void *arg;//for printing
    struct STATE *child;//the address in root array where this.children start
    int childCount;
    
    //for processing
    char whitelisted;//1 if true
    struct STATE *parent;//what actually being used
    int parentCount;
    double *parentChance;//NAN = rejected, neg = accepted, pos = undecided

} STATE ;
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
void graph_close(){
    for(int i = 0 ; i < graph.root.len ; i++){
        if(graph.root.ptr[i].parentChance == NULL) continue;
        free(graph.root.ptr[i].parentChance);
        graph.root.ptr[i].parentChance = NULL;
    }
    
    free(graph.root.ptr);
    graph.root.ptr = NULL;

    printf("graph closed\n");
}
void __test_graph_init(){
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
    graph.root.ptr[0].childCount = 2;
    graph.root.ptr[0].child = &(graph.root.ptr[1]);
    graph.root.ptr[1].childCount = 3;
    graph.root.ptr[1].child = &(graph.root.ptr[3]);
    graph.root.ptr[2].childCount = 1;
    graph.root.ptr[2].child = &(graph.root.ptr[4]);

    //parent
    int parentId[] =    {-1,0,0,1,1,1};
    int parentCount[] = {-1,1,1,1,2,1};
    for(int i = 1; i<=5; i++){
        graph.root.ptr[i].parentCount = parentCount[i];
        graph.root.ptr[i].parent = graph.root.ptr + parentId[i];
        graph.root.ptr[i].parentChance = (double*)malloc(sizeof(double)*parentCount[i]);
        for(int j = 0; j<graph.root.ptr[i].parentCount; j++){
            graph.root.ptr[i].parentChance[j] = i*100 + j;    
        }
    }
    
  
    atexit(graph_close);
}



struct HEAP{
    STATE *data;
    struct HEAP *left;
    struct HEAP *right;
}candidate;




typedef struct {
    int graphId;
    short *queued;//a bool array for each node indicating wheter it has already been processed
    FILE *outFile;
} JSON ;
JSON *json_init(char *filename){
    JSON *json = (JSON *)calloc(1,sizeof(JSON));
    json->graphId = 0;
    json->queued = NULL;
    json->outFile = fopen(filename,"w");
    fprintf(json->outFile,"[\n");
    return json;
}
void json_close(JSON *json){
    fprintf(json->outFile,"]\n");
    fclose(json->outFile);
    free(json);
}
/** also assign Id */
/* done */void __json_printGraphNodes(JSON *json){
    fprintf(json->outFile,"        \"nodes\":[\n");
    

    typedef struct QUEUE {
        int depth;
        STATE *node;
        struct QUEUE *next;
    } QUEUE;
    QUEUE *queueHead = NULL, *_queue, *queueTail = NULL;
    memset(json->queued,0,graph.root.len); 
   

    STATE *current = graph.root.ptr;
    short currentNodeId = 0;
    short currentDepth = 0;
    while(1){
        //print current
        fprintf(json->outFile,"           ");
        if(currentNodeId)fprintf(json->outFile,",");
        fprintf(json->outFile,"{\"depth\":%d,\"price\":%lf,\"succesR\":%lf,\"detail\":\"%s\",\"accept\":\"%d\"}\n"
            ,currentDepth
            ,current->price
            ,current->successChance
            ,current->msg
            ,(int)current->whitelisted
        );
        
        
        //append children into queue(if not already in queue)
        currentDepth++;
        for(int i = 0; i<current->childCount; i++){
            if( json->queued[current->child + i - graph.root.ptr] ) continue;
            //create queue node
            _queue = (QUEUE *)malloc(sizeof(QUEUE));
            _queue->node = current->child + i;
            _queue->depth = currentDepth;
            _queue->next = NULL;

            //append queue node
            if(queueTail == NULL){ 
                queueHead = queueTail = _queue;
            } else {
                queueTail->next = _queue;
                queueTail = queueTail->next;
            }
            _queue = NULL;

            //mark node by assigning id
            currentNodeId++;
            json->queued[current->child + i - graph.root.ptr] = currentNodeId;

        }
        
        //pop QUEUE, set to current
        if(queueHead == NULL)break;        
        current = queueHead->node;
        currentDepth = queueHead->depth;

        if(queueHead == queueTail){
            free(queueHead);
            queueHead = queueTail = NULL;
        } else {
            _queue = queueHead;
            queueHead = queueHead->next;
            free(_queue);
            _queue = NULL;
        }

    } 

    fprintf(json->outFile,"        ],\n");
}
/* done */void __json_printGraphLinks(JSON *json){
    fprintf(json->outFile,"        \"links\":[\n");
    

    typedef struct QUEUE {
        STATE *node;
        struct QUEUE *next;
    } QUEUE;
    QUEUE *queueHead = NULL, *queueTail = NULL, *_queue;
   

    STATE *current = graph.root.ptr;
    short nodeid = 0;
    while(1){
        
        //print current links
        STATE *currentParent;
        for(int i = 0; i<current->parentCount; i++){
            currentParent = current->parent + i;

            fprintf(json->outFile,"           ");
            if(nodeid) fprintf(json->outFile,",");
            fprintf(json->outFile,"{\"src\":%d,\"dst\":%d,\"chance\":%lf}\n"
                ,abs(json->queued[currentParent - graph.root.ptr])
                ,abs(json->queued[current - graph.root.ptr])  
                ,current->parentChance[i]
            );
            nodeid++;
        }

        //append children into queue(if not already in queue)
        STATE *currentChild;
        for(int i = 0; i<current->childCount; i++){
            currentChild = current->child + i;
            if( json->queued[currentChild - graph.root.ptr] <= 0) continue;

            //create queue node
            _queue = (QUEUE *)malloc(sizeof(QUEUE));
            _queue->node = currentChild;
            _queue->next = NULL;

            //append queue node
            if(queueTail == NULL){ 
                queueHead = queueTail = _queue;
            } else {
                queueTail->next = _queue;
                queueTail = queueTail->next;
            }
            _queue = NULL;

            //mark node by assigning id 
            json->queued[currentChild - graph.root.ptr] *= -1;

        }

        
        //pop QUEUE, set to current
        if(queueHead == NULL)break;        
        current = queueHead->node;
        if(queueHead == queueTail){
            free(queueHead);
            queueHead = queueTail = NULL;
        } else {
            _queue = queueHead;
            queueHead = queueHead->next;
            free(_queue);
            _queue = NULL;
        }

    } 

    fprintf(json->outFile,"        ]\n");
}
/** initiallized json->queued */
/* done */void json_printGraph(JSON *json){
    if(json->graphId == 0) fprintf(json->outFile,"    {\n");
    else fprintf(json->outFile,"    ,{\n");
    
    json->queued = (short *)calloc(graph.root.len,sizeof(short));
    __json_printGraphNodes(json);
    __json_printGraphLinks(json);
    free(json->queued);
    json->queued = NULL;

    fprintf(json->outFile,"    }\n");
    json->graphId++;
}
void json_printHeap();



/* done */void iniGlobalVariable(int piece, int mainstat, int *substat, int substat_len){
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



int main(){
    //initialize constant variable
    // int substat[] = {STAT_CD,STAT_ATKP};
    // iniGlobalVariable(PIECE_BODY,STAT_CR,substat,sizeof(substat)/sizeof(int)); 
    // struct COST cost2;
    // struct CHANCE chance2;
    // memcpy(&cost2,&cost,sizeof(struct COST));
    // memcpy(&chance2,&chance,sizeof(struct CHANCE));

    
    __test_graph_init();


    //
    JSON *json = json_init("mydata.json");
    json_printGraph(json);
    json_close(json);
    
    

    

    
    
    return 0;
}
/* 
    


*/