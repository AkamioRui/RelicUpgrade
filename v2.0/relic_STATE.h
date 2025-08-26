#ifndef relic_STATE
#define relic_STATE

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "relic_JSON.h"
#include "relic_HEAP.h"
#include "relic_globalConstant.h"

typedef enum STATUS{
    STATUS_BANNED = -1,
    STATUS_UNKOWN = 0,
    STATUS_ACCEPTED = 1
} STATUS;
typedef struct STATE{
    double price;//the average price
    double successChance;
    char msg[16];
    STATUS whitelisted;//1 if true

    void *arg;//for printing
    struct STATE *child;//the address in root array where this.childrens is located contiguously 
    int childCount;

    //for processing
    struct STATE *parent;//the address in root array, parent node are  located contiguously 
    int parentCount;
    double *parentChance;//NAN = rejected, neg = accepted, pos = undecided
    HEAP_NODE *heapNode;//deallocated by heap

} STATE ;
typedef struct STATE_array{
    STATE *ptr;
    int len;
} STATE_array;
struct {
    //{
    //     root
    //     ,three.start //1 node
    //     ,four.start //1 node
    //     ,three.combination // c(#remainingSubstat,3) nodes
    //     ,three.good // #goodSub nodes
    //     ,four.good // #goodSub nodes
    //     ,three.upgrade[4] // #goodSub*(5*6/2)-1 nodes //arithmatic sum
    //     ,four.upgrade[4] // #goodSub*(6*7/2)-1 nodes //arithmatic sum
    // }
    STATE_array root;//len is the entire nodes
    

    //checkpoints
    //the following are just for references, it doesnt allocate memory
    struct {
        STATE_array start;
        STATE_array combination[3];//use arg to temp store substatId[]
        STATE_array good;
        STATE_array upgrade[4];//index = number of good substat -1, some entry might be NULL if #goodStat < 4
    } three;

    struct {
        STATE_array start;
        STATE_array good;
        STATE_array upgrade[4];
    } four;

    HEAP *heap;
} graph;
JSON *STATE_file;


#define debug
#ifdef debug
    #define debug_close
    #define debug_init_upgrade
    #define int_graph_forwardPeak//return whether graph.root is modified

#endif







//printing
STATE *STATE_getChild(STATE *node, int index);
int STATE_getChildCount(STATE *node);
void STATE_fprintNodeD(FILE *outFile,STATE *node);
void STATE_fprintLinkD(FILE *outFile, STATE *parentNode, STATE *childNode);
void STATE_clearArg(STATE *root);
__json_printSpannigTree_generic(STATE)


//utility
int isMoreEfficient(void *a,void *b);
// initGlobalVariable(PIECE piece, STAT mainstat, STAT *substat, int substat_len)

//count the number of allocated memory needed, then allocate the graph
//resposible only for completing all graph data, not each indiviual node
void graph_init();
void graph_init_root(); //fill in all detail for all node in start
void graph_init_start(int branch); //fill in all detail for all node in start
void graph_init_combination();
void graph_init_good(int branch);
void graph_init_upgrade(int branch, int index, double pwin);
void graph_close();

//return whether graph.root is modified
int graph_forwardPeak();
void graph_eliminateDefective();



#ifdef debug
    void graph_printState(FILE *file){
        for(int i = 0 ; i<graph.root.len; i++){
            STATE *node = graph.root.ptr + i;
            
            //the node
            fprintf(file,"-%d-> {%s:%.2lf/%.2lf(%c)} -%d-> ",
                node->parentCount,
                node->msg, 
                node->successChance, 
                node->price, 
                node->whitelisted?'v':'x',
                node->childCount
            );

            //each parent
            fprintf(file,"\n\t");
            for(int pid = 0; pid<node->parentCount; pid++){
                STATE *parent = node->parent+pid;
                fprintf(file,"{(%.2lf)%s:%.2lf/%.2lf(%c)} ",
                    node->parentChance[pid],
                    parent->msg, 
                    parent->successChance, 
                    parent->price, 
                    parent->whitelisted?'v':'x'
                );

            }

            fprintf(file,"\n\t");
            for(int cid = 0; cid<node->childCount; cid++){
                STATE *child = node->child+cid;
                fprintf(file,"{%s:%.2lf/%.2lf(%c)} ",
                    child->msg, 
                    child->successChance, 
                    child->price, 
                    child->whitelisted?'v':'x'
                );

            }
            fprintf(file,"\n");

    
        }
    }
    //will deallocate itself
    //pyramid when it can only upgrade 4 times + the goodnode pointing to i
    void graph_init_test_pyramid4(int goodSubCount){
        memset(&graph,0,sizeof(graph));

        //assigning heap
        graph.heap = HEAP_init((HEAP_Compare *)isMoreEfficient);
        
        //assigning len
        graph.root.len = 1;
        graph.three.start.len = 0;
        graph.three.combination[0].len =0; 
        graph.three.combination[1].len =0; 
        graph.three.combination[2].len =0; 
        graph.three.good.len = 0;
        graph.three.upgrade[0].len =5*6/2-1; 
        graph.three.upgrade[1].len =0; 
        graph.three.upgrade[2].len =0; 
        graph.three.upgrade[3].len =0; 
        graph.four.start.len = 0;
        graph.four.good.len = 0;
        graph.four.upgrade[0].len =0; 
        graph.four.upgrade[1].len =0; 
        graph.four.upgrade[2].len =0; 
        graph.four.upgrade[3].len =0; 
        
        graph.root.len += 
          graph.three.start.len
        + graph.three.combination[0].len
        + graph.three.combination[1].len
        + graph.three.combination[2].len
        + graph.three.good.len
        + graph.three.upgrade[0].len
        + graph.three.upgrade[1].len
        + graph.three.upgrade[2].len
        + graph.three.upgrade[3].len
        + graph.four.start.len
        + graph.four.good.len
        + graph.four.upgrade[0].len
        + graph.four.upgrade[1].len
        + graph.four.upgrade[2].len
        + graph.four.upgrade[3].len ;
        
        
        //assiging ptr
        graph.root.ptr = (STATE *)calloc(graph.root.len,sizeof(STATE));
        STATE *availablePointer = graph.root.ptr + 1;
        #define giveSpaceTo(checkpoint) if(checkpoint.len){\
            checkpoint.ptr = availablePointer;\
            availablePointer += checkpoint.len;\
        }    
        giveSpaceTo(graph.three.start)
        giveSpaceTo(graph.four.start)
        giveSpaceTo(graph.three.combination[0])
        giveSpaceTo(graph.three.combination[1])
        giveSpaceTo(graph.three.combination[2])
        giveSpaceTo(graph.three.good)
        giveSpaceTo(graph.four.good)
        giveSpaceTo(graph.three.upgrade[0])
        giveSpaceTo(graph.three.upgrade[1])
        giveSpaceTo(graph.three.upgrade[2])
        giveSpaceTo(graph.three.upgrade[3])
        giveSpaceTo(graph.four.upgrade[0])
        giveSpaceTo(graph.four.upgrade[1])
        giveSpaceTo(graph.four.upgrade[2])
        giveSpaceTo(graph.four.upgrade[3])
        
        #undef giveSpaceTo
    
        graph.root.ptr->price = cost.create;
        graph.root.ptr->successChance = 0;
        sprintf(graph.root.ptr->msg,"root");
        graph.root.ptr->whitelisted = 0;//always 0
        
        graph.root.ptr->arg = NULL;
        graph.root.ptr->child = graph.three.start.ptr;
        graph.root.ptr->childCount = 2;
        
        graph.root.ptr->parentCount = 0;
        graph.root.ptr->parent = NULL;
        graph.root.ptr->parentChance = NULL;
        graph.root.ptr->heapNode = NULL;
        

        //modified for testing
        graph.root.ptr->child = graph.three.upgrade[0].ptr;
        graph.root.ptr->childCount = 2;
        //to avoid segfault, three.good[0] = root
        graph.three.good.ptr = graph.root.ptr;
        graph_init_upgrade(3, 0, goodSubCount/4.f);
        

        
        json_printGraph(STATE_file,graph.root.ptr,(JSON_PRINT_FUNC *)__json_printSpanningTree_STATE,"created");
        json_printGraph(HEAP_file,graph.heap->root,(JSON_PRINT_FUNC *)__json_printSpanningTree_HEAP_NODE,"created");
        printf("graph initialized\n");
    }

    
#endif


//branch is three or four
void graph_init_upgrade(int branch, int index, double pwin){
    

    STATE *upgrade;
    STATE *goodNode;
    int maxLayer; //layer number refers to how many node in each layer 
    switch (branch){
        case 3:
            // assert(index < graph.three.good.len);
            goodNode = graph.three.good.ptr + index;
            upgrade = graph.three.upgrade[index].ptr;
            maxLayer = 5; //0,1,2,3,4 upgrade
        break;

        case 4:
            // assert(index < graph.four.good.len);
            goodNode = graph.four.good.ptr + index;
            upgrade = graph.four.upgrade[index].ptr;
            maxLayer = 6; //0,1,2,3,4,5,6 upgrade
        break;
        
        default:
            printf("invalid branch");
            assert(0);
        break;
    }
    assert(upgrade);
    assert(goodNode);

    STATE *curLayer = upgrade;
    for(int l = 2; l<=maxLayer ; l++){
        char parentIsGood = (l==2)
            ,lastLayer = (l==5);

        int curCost = cost.upgrade[4 - (maxLayer-l)];
        STATE *parentLayer = curLayer - (l-1);
        STATE *childLayer = curLayer + l;

        for(int up = 0; up<l; up++){
            STATE *curNode = curLayer + up;

            curNode->price = curCost;
            curNode->successChance = lastLayer*((index + 1 + up)>=threshold);
            sprintf(curNode->msg,"up %d,%d",l,up);
            curNode->whitelisted = 0;//1 if true

            curNode->arg = NULL;
            curNode->child = lastLayer? NULL : childLayer + up;
            curNode->childCount = (!lastLayer)*2;
            
            curNode->parentCount = parentIsGood? 1 : (0 < up)+(up < l-1);
            curNode->parent = parentIsGood? goodNode : 
                (up==0? parentLayer: parentLayer + up - 1);
            if(curNode->parentCount>0) curNode->parentChance = (double *)
            calloc(curNode->parentCount,sizeof(double));
            else curNode->parentChance = NULL;
            if(parentIsGood){
                curNode->parentChance[0] = (!up)*(1-pwin) + (up)*pwin;
            } else if(up == 0){
                curNode->parentChance[0] = (1-pwin);
            } else if(up == l-1){
                curNode->parentChance[0] = pwin;
            } else {
                curNode->parentChance[0] = pwin;
                curNode->parentChance[1] = (1-pwin);
            }
            
            
            if(lastLayer && curNode->successChance){
                curNode->heapNode = HEAP_add(graph.heap,curNode);
            } else curNode->heapNode = NULL;


        }
        curLayer = childLayer;
    }

    #ifdef debug_init_upgrade
        printf("graph.%d.upgrade[%d] initialized\n",branch,index);
    #endif
}

//deallocate all the memory allocated inside graph
void graph_close(){
    HEAP_close(&graph.heap);

    STATE *node = graph.root.ptr;
    for(int i = 0 ; i < graph.root.len ; i++){
        if(node[i].arg){
            free(node[i].arg);
            node[i].arg = NULL;
        }
        
        if(node[i].parentChance){
            free(node[i].parentChance);
            node[i].parentChance = NULL;
        }
        
        
    }
    free(graph.root.ptr);
    graph.root.ptr = NULL;

    #ifdef debug_close
        printf("graph closed\n");
    #endif
}


//will propagate price and chance from node to its parent. note that node's price and chance is irrelevant, only its parents and their chance
int graph_propagate_from(STATE *node,double price, double successChance){   
    int rootIsModified = 0;

    for(int i = 0 ; i<node->parentCount; i++){
        STATE *parent = node->parent + i;     
        double chance = node->parentChance[i];
        
        //update all each parents data
        double d_price = price * chance;
        double d_successChance = successChance * chance;
        parent->price += d_price;
        parent->successChance += d_successChance;


        //make sure heap is accurate
        if(parent == graph.root.ptr){//do nothing
            rootIsModified++;
        } else {
            HEAP_NODE *parentHNode = parent->heapNode;
            int code = 
                ((parent->whitelisted != 0)<<1) + 
                (parentHNode != NULL);
            switch (code){ 
                case 0b00://no whitelisted, not in heap
                parent->heapNode = HEAP_add(graph.heap, parent); 
                break;

                case 0b01://no whitelisted, in heap
                if(parentHNode == graph.heap->root 
                    || isMoreEfficient((STATE *)parentHNode->parent->data,(STATE *)parentHNode->data)){
                    HEAP_normalizeDown(graph.heap,parentHNode);
                } else {
                    HEAP_normalizeUp(graph.heap,parentHNode);
                }
                break;

                case 0b10://whitelisted, not in heap
                rootIsModified += graph_propagate_from(parent ,d_price,d_successChance);
                break;

                case 0b11://whitelisted, but in heap (illegal)
                default://invalid code
                    assert(0);
                break;
            }
        }
        
        
        
        
        // if(parent->whitelisted){//node is whitelisted, it is not and must not be in the heap 
            
        // } else {//node is not whitelisted, it must be placed in the heap and its position must be updated
        //     HEAP_NODE *parentHNode = parent->heapNode;
        //     if(parentHNode) {//parent heap node location is updated
        //         if(parentHNode == graph.heap->root 
        //             || isMoreEfficient((STATE *)parentHNode->parent->data,(STATE *)parentHNode->data)){
        //             HEAP_normalizeDown(graph.heap,parentHNode);
        //         } else {
        //             HEAP_normalizeUp(graph.heap,parentHNode);
        //         }
        //     } else {//parent is not on the heap
        //         parent->heapNode = HEAP_add(graph.heap, parent); 
        //     }
        // }

        
        
        
    }

    return rootIsModified;
}
//return whether graph.root is modified
int graph_forwardPeak(){
    int rootIsModified = 0;

    //pop peak
    STATE *peakNode = HEAP_pop(graph.heap);
    peakNode->heapNode = NULL;
    peakNode->whitelisted = STATUS_ACCEPTED;
    
    
    double peakPrice = peakNode->price;
    double peakRate = peakNode->successChance;
    rootIsModified += graph_propagate_from(peakNode,peakPrice,peakRate);
   
    // for(int i = 0 ; i<peakNode->parentCount; i++){
    //     STATE *parent = peakNode->parent + i;     
    //     double chance = peakNode->parentChance[i];

    //     //forward peak into each parents
    //     parent->price += peakPrice * chance;
    //     parent->successChance += peakRate * chance;

    //     //if not already in heap, add peak parent to heap
    //     HEAP_NODE *parentHNode = parent->heapNode;
    //     if(parentHNode) {//parent heap node changes
    //         if(parentHNode == graph.heap->root 
    //             || isMoreEfficient((STATE *)parentHNode->parent->data,(STATE *)parentHNode->data)){
    //             HEAP_normalizeDown(graph.heap,parentHNode);
    //         } else {
    //             HEAP_normalizeUp(graph.heap,parentHNode);
    //         }
    //     } else {//parent is not on the heap
    //         if(parent ==graph.root.ptr){
    //             rootIsModified++;
    //         } else {
    //             parent->heapNode = HEAP_add(graph.heap, parent); 
    //         } 
    //     }
        
    // }

    
    #ifdef int_graph_forwardPeak//return whether graph.root is modified
    char tmp[100];
    sprintf(tmp,"propagated %s",peakNode->msg);
    json_printGraph(STATE_file,graph.root.ptr,(JSON_PRINT_FUNC *)__json_printSpanningTree_STATE,tmp);
    json_printGraph(HEAP_file,graph.heap->root,(JSON_PRINT_FUNC *)__json_printSpanningTree_HEAP_NODE,tmp);
    printf("%s\n",tmp);
    #endif 

    
    return rootIsModified;
}
int isMoreEfficient(void *a,void *b){
    STATE *ap = (STATE *)a;
    STATE *bp = (STATE *)b;
    return (ap->successChance/ap->price) > (bp->successChance/bp->price);
}

//for printing
STATE *STATE_getChild(STATE *node, int index){
    return node->child + index;
}
int STATE_getChildCount(STATE *node){
    return node->childCount;
}
void STATE_fprintNodeD(FILE *outFile,STATE *node){
    
    fprintf(outFile,"{");
    fprintf(outFile," \"detail\":\"%s\"",node->msg);
    fprintf(outFile,",\"price\":%.2lf"  ,node->price);
    fprintf(outFile,",\"succesR\":%.2lf",node->successChance);
    fprintf(outFile,",\"accept\":%d"    ,node->whitelisted);
    fprintf(outFile,",\"inHeap\":%d"    ,node->heapNode != NULL);
    fprintf(outFile,"}");
}
void STATE_fprintLinkD(FILE *outFile, STATE *parentNode, STATE *childNode){
    if(!parentNode ){
        fprintf(outFile,"{\"chance\":0}");
        return;
    }
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


//HEAP section, filling the global variable
void HEAP_NODE_fprintNodeD(FILE *outFile,HEAP_NODE *node){
    STATE *node_data = (STATE *)node->data;
    fprintf(outFile,"{");
    fprintf(outFile," \"detail\":\"%s\"",node_data->msg);
    fprintf(outFile,",\"price\":%.2lf"  ,node_data->price);
    fprintf(outFile,",\"succesR\":%.2lf",node_data->successChance);
    fprintf(outFile,",\"accept\":%d"    ,node_data->whitelisted);
    fprintf(outFile,"}");

}
void HEAP_NODE_fprintLinkD(FILE *outFile, HEAP_NODE *parentNode, HEAP_NODE *childNode){
    if(childNode->parent)fprintf(outFile,"{\"msg\":\"%s\"}",((STATE*)childNode->parent->data)->msg);
    else fprintf(outFile,"{\"msg\":\"_\"}");
}


#undef debug
#endif
//drawing a STATE graph
// graph_init_test();
// JSON *mainGraphJson = json_init("mydata.json");
// json_printGraph(
//     mainGraphJson,
//     (void *)graph.root.ptr,
//     (void (*)(void *, FILE *, FILE *, int *, int *))__json_printSpanningTree_STATE
// );
// json_close(mainGraphJson);

/* 
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

*/