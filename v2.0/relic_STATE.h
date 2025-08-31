#ifndef relic_STATE
#define relic_STATE

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "relic_JSON.h"
#include "relic_HEAP.h"
#include "relic_globalConstant.h"

#define debug
#ifdef debug
    #define debug_close_log
    #define debug_init
        #define debug_init_log
        #define debug_init_print
    #define debug_init_root_log
    #define graph_init_start_log
    #define graph_init_combination_log
    #define graph_init_good_3_log
    #define graph_init_good_4_log
    #define debug_init_upgrade_log
    // #define debug_propagate_peak
        #define debug_propagate_peak_log
        #define debug_propagate_peak_print

    FILE *logfile;
    #define __printHeap
    // #define __printHeap json_printGraph(HEAP_file,graph.heap->root,(JSON_PRINT_FUNC *)__json_printSpanningTree_HEAP_NODE,tmp);
    #define __printGraph
    // #define __printGraph json_printGraph(STATE_file,graph.root.ptr,(JSON_PRINT_FUNC *)__json_printSpanningTree_STATE,tmp);
#endif

typedef enum STATUS{
    STATUS_BANNED = -1,
    STATUS_UNKOWN = 0,
    STATUS_ACCEPTED = 1
} STATUS;
typedef struct STATE{
    double price;//the average price
    double successChance;
    char msg[24];
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
struct GRAPH{
    STATE_array root;//len is the entire nodes
    
    //checkpoints. It doesnt allocate memory, only points to it
    //index = number of good sub. start at 0
    struct {
        STATE_array start;
        STATE_array combination[4];
        STATE_array good;//len = 5
        STATE_array upgrade[5];//the [0].len always 0;
    } three;

    struct {
        STATE_array start;
        STATE_array good;//len = 5
        STATE_array upgrade[5];//the [0].len always 0;
    } four;

    HEAP *heap;
} graph;
JSON *STATE_file;









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
void graph_init_root(); 
void graph_init_start(int branch); //fill in all detail for all node in start
void graph_init_combination();//use arg to store substatId[3], 
void graph_init_good_3();//for threebranch, use combination.arg to get parent chance and deallocate it
void graph_init_good_4();
void graph_init_upgrade(int branch, int index, double pwin);
void graph_close();

//return whether graph.root is modified
int graph_propagate_peak();
int graph_propagate_from(STATE *node,double price, double successChance);


//from the top, if a whitelisted node doesnt have a whitelisted parent, then it no longer be whitelisted
void graph_trimWhitelist();

void graph_init(){
    memset(&graph,0,sizeof(graph));
    
    //assigning len
    graph.root.len = 1;
    graph.three.start.len = 1;
    graph.four.start.len = 1;

    for(int G=chance.substatGoodCount, k1=1, i=0; i<4; i++){
        //k1 = c(G,0)
        graph.three.combination[i].len = k1;
        k1 = k1 * (G-i)/(1+i);// =G/1
    } 
    for(int B=chance.substatBadCount, k2=1, i=4-1; i>=0; i--){
        //k1 = c(B,0)
        graph.three.combination[i].len *= k2;
        k2 = k2 * (B-4+1+i)/(4-i);// =B/1
    }

    int idxMax = 1 + 
    (chance.substatGoodCount<4?chance.substatGoodCount:4);
    graph.three.good.len = idxMax;
    graph.four.good.len  = idxMax;
    
    for(int i = chance.substatGoodCount<4?chance.substatGoodCount:4; i>=1; i--){
        graph.three.upgrade[i].len =5*6/2-1; 
        graph.four.upgrade[i].len =6*7/2-1; 
    }
    
    //assign Ptr
    graph.root.len = graph.root.len
        + graph.three.start.len
        + graph.four.start.len
        + graph.three.combination[0].len
        + graph.three.combination[1].len
        + graph.three.combination[2].len
        + graph.three.combination[3].len
        + graph.three.good.len
        + graph.four.good.len
        + graph.three.upgrade[0].len
        + graph.three.upgrade[1].len
        + graph.three.upgrade[2].len
        + graph.three.upgrade[3].len
        + graph.three.upgrade[4].len
        + graph.four.upgrade[0].len
        + graph.four.upgrade[1].len
        + graph.four.upgrade[2].len
        + graph.four.upgrade[3].len 
        + graph.four.upgrade[4].len 
    ;
    
    
    //assiging ptr
    graph.root.ptr = (STATE *)calloc(graph.root.len,sizeof(STATE));
    STATE *availablePointer = graph.root.ptr + 1;
    #define giveSpaceTo(checkpoint) if(checkpoint.len){\
        checkpoint.ptr = availablePointer;\
        availablePointer += checkpoint.len;\
    }    
    giveSpaceTo(graph.three.start);
    giveSpaceTo(graph.four.start);
    giveSpaceTo(graph.three.combination[0]);
    giveSpaceTo(graph.three.combination[1]);
    giveSpaceTo(graph.three.combination[2]);
    giveSpaceTo(graph.three.combination[3]);
    giveSpaceTo(graph.three.good);
    giveSpaceTo(graph.four.good);
    giveSpaceTo(graph.three.upgrade[0]);
    giveSpaceTo(graph.three.upgrade[1]);
    giveSpaceTo(graph.three.upgrade[2]);
    giveSpaceTo(graph.three.upgrade[3]);
    giveSpaceTo(graph.three.upgrade[4]);
    giveSpaceTo(graph.four.upgrade[0]);
    giveSpaceTo(graph.four.upgrade[1]);
    giveSpaceTo(graph.four.upgrade[2]);
    giveSpaceTo(graph.four.upgrade[3]);  
    giveSpaceTo(graph.four.upgrade[4]);   
    #undef giveSpaceTo

    #ifdef debug_init
        char *tmp = "created";
        #ifdef debug_init_log
            #define __printData(node)\
            printf("%3d,%14llu:"#node"\n",graph.node.len,graph.node.ptr-graph.root.ptr);
            __printData(root);
            __printData(three.start);
            __printData(four.start);
            __printData(three.combination[0]);
            __printData(three.combination[1]);
            __printData(three.combination[2]);
            __printData(three.combination[3]);
            __printData(three.good);
            __printData(four.good);
            __printData(three.upgrade[0]);
            __printData(three.upgrade[1]);
            __printData(three.upgrade[2]);
            __printData(three.upgrade[3]);
            __printData(three.upgrade[4]);
            __printData(four.upgrade[0]);
            __printData(four.upgrade[1]);
            __printData(four.upgrade[2]);
            __printData(four.upgrade[3]);
            __printData(four.upgrade[4]);
            #undef __printData
            printf("graph initialized\n");
        #endif
        #undef debug_init_log
    #endif
    
    //assigning heap
    graph.heap = HEAP_init((HEAP_Compare *)isMoreEfficient);

    graph_init_root();
    graph_init_start(3); 
    graph_init_start(4); 
    graph_init_combination();
    graph_init_good_3();//delete comb.arg
    graph_init_good_4();
    // graph_init_goo and deallocate itd(4); and deallocate it
    // 
    for(int index = 1; index < graph.three.good.len; index++){
        graph_init_upgrade(3, index, index/4.f);
        graph_init_upgrade(4, index, index/4.f);
    }

    #ifdef debug_init
        #ifdef debug_init_print
            __printHeap;
            __printGraph;
        #endif
        #undef debug_init_print
    #endif
    #undef debug_init
    
}
void graph_init_root(){
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
    
    #ifdef debug_init_root_log
        printf("graph root initialized\n");
    #endif 
    #undef debug_init_root_log
} 
void graph_init_start(int branch){
    STATE *node;
    switch(branch){
        case 3: 
            node = graph.three.start.ptr;

            // node->price = 0;
            // node->successChance = 0;
            sprintf(node->msg,"start 3");
            // node->whitelisted = 0;
            
            // node->arg = NULL;
            node->child = graph.three.combination[0].ptr;
            node->childCount = 
                  graph.three.combination[0].len 
                + graph.three.combination[1].len 
                + graph.three.combination[2].len
                + graph.three.combination[3].len
            ;
            
            node->parentCount = 1;
            node->parent = graph.root.ptr;
            node->parentChance = (double *)malloc(sizeof(double));
                node->parentChance[0]=
                chance.mainstat *(1 - chance.startFour);
            // node->heapNode = NULL;
        break;

        case 4: 
            node = graph.four.start.ptr; 

            // node->price = 0;
            // node->successChance = 0;
            sprintf(node->msg,"start 4");
            // node->whitelisted = 0;
            
            // node->arg = NULL;
            node->child = graph.four.good.ptr;
            node->childCount = graph.four.good.len;
            
            node->parentCount = 1;
            node->parent = graph.root.ptr;
            node->parentChance = (double *)malloc(sizeof(double));
                node->parentChance[0]=
                chance.mainstat *chance.startFour;
            // node->heapNode = NULL;

        break;

        default: assert(0); break;//invalid branch
    }

    


    #ifdef graph_init_start_log
        printf("graph.%d.start initialized\n",branch);
    #endif 
    #undef graph_init_start_log
}
void graph_init_combination(){
    

    //for each combinaton/layer
    for(int idx = 0; idx<=3; idx++){
        if(!graph.three.combination[idx].len)break;
        STATE *layerArr = graph.three.combination[idx].ptr;
        int layerArrLen = graph.three.combination[idx].len;

        //setting up the 3-loop
        int nol = -1, *i2p, *i3p;
        int i1, i2, i3;
        int i1Len, i2Len, i3Len;
        STAT *i1Src, *i2Src, *i3Src;
        switch(idx){
            #define assign_as_(id,quality) \
            id##Len = chance.substat##quality##Count;\
            id##Src = chance.substat##quality;
            
            case 0:// bad/bad/bad
            assign_as_(i1,Bad);
            assign_as_(i2,Bad);
            assign_as_(i3,Bad);
            i2p = &i1;
            i3p = &i2;
            break;

            case 1:// good/bad/bad
            assign_as_(i1,Good);
            assign_as_(i2,Bad);
            assign_as_(i3,Bad);
            i2p = &nol;
            i3p = &i2;
            break;

            case 2:// good/good/bad
            assign_as_(i1,Good);
            assign_as_(i2,Good);
            assign_as_(i3,Bad);
            i2p = &i1;
            i3p = &nol;
            break;

            case 3:// good/good/good
            assign_as_(i1,Good);
            assign_as_(i2,Good);
            assign_as_(i3,Good);
            i2p = &i1;
            i3p = &i2;
            break;

            default: assert(0); break; 
            #undef assign_as_
            
        }
        
        //for each node
        int nodeId = 0;//reset per layer
        for(i1 = 0      ; i1<i1Len; i1++){
        for(i2 = *i2p +1; i2<i2Len; i2++){
        for(i3 = *i3p +1; i3<i3Len; i3++){
            STAT substat[] = {i1Src[i1], i2Src[i2], i3Src[i3]};

            assert(nodeId<graph.three.combination[idx].len);
            STATE *node = layerArr+ nodeId++;
            
            node->price = cost.upgrade[0];
            // node->successChance = 0;
            sprintf(node->msg,"C%2d,%2d:%s-%s-%s",
                idx,
                nodeId -1,
                STATname[substat[0]],
                STATname[substat[1]],
                STATname[substat[2]]
            );
            node->whitelisted = 0;
            
            node->arg = (STAT*)malloc(sizeof(substat));
            memcpy(node->arg,substat,sizeof(substat));
            
            node->child = graph.three.good.ptr+idx;
            node->childCount = 1+( idx+1 <= graph.three.good.len-1);
            
            node->parentCount = 1;
            node->parent = graph.three.start.ptr;
            node->parentChance = (double *)malloc(sizeof(double));
            node->parentChance[0] = 0;//will hold (1/... +1/... +...)//the nominator are all the same so, later
            //iterating over substat
            int ttlw1 = chance.substatWeightTotal;
            for(int p1 = 0; p1<3; p1++){
                int ttlw2 = ttlw1-chance.substatWeight[substat[p1]];
                for(int p2 = 0; p2<3; p2++){
                    if(p2 == p1) continue;
                    int ttlw3 = ttlw2-chance.substatWeight[substat[p2]];
                    node->parentChance[0] += 1.f/ttlw1/ttlw2/ttlw3;
                }
            }
            node->parentChance[0] *= 1.f
                *chance.substatWeight[substat[0]]
                *chance.substatWeight[substat[1]]
                *chance.substatWeight[substat[2]]
            ;//multiplied by nominator
            // node->heapNode = NULL;


        }
        }
        }

    }

    
    
    
    
    #ifdef graph_init_combination_log
        double sum = 0;
        int len = graph.three.combination[0].len
        +graph.three.combination[1].len
        +graph.three.combination[2].len
        +graph.three.combination[3].len
        ;
        for(int i = 0; i<len; i++)sum += graph.three.combination[0].ptr[i].parentChance[0];
        printf("graph.3.combination initialized; p = %lf\n",sum);
    #endif 
    #undef graph_init_combination_log
}
void graph_init_good_3(){
    #define addtoChecksum 
    #ifdef graph_init_good_3_log
        double checksum = 0;    
        
        #define addtoChecksum checksum += totalChance;
        
        
    #endif 
    
    
    for(int idx=0; idx<graph.three.good.len; idx++){
        STATE *node = graph.three.good.ptr + idx;

        node->price = cost.upgrade[1];
        node->successChance = 0;
        ///*  */sprintf(node->msg,"Good-%d:(%.6lf)");//below
        node->whitelisted = 0;
        
        node->arg = NULL;
        node->child = graph.three.upgrade[idx].ptr;
        node->childCount = graph.three.upgrade[idx].len?2:0;//the first 2 node
        
        int hasDown  = 0<=idx - 1 ;
        int hasEqual = idx<=3;
        node->parentCount = 
             (hasDown ? graph.three.combination[idx-1].len:0)
            +(hasEqual? graph.three.combination[idx].len  :0)
        ;
        node->parent = hasDown?
            graph.three.combination[idx-1].ptr:
            graph.three.combination[idx].ptr
        ;

        node->parentChance = (double *)malloc(sizeof(double)*node->parentCount);
        int pCid = 0;
        double totalChance = 0;//for detail/msg

        int goodTtlW = 0;
        for(int i = 0; i<chance.substatGoodCount; i++)goodTtlW+=
        chance.substatWeight[chance.substatGood[i]];
        int badTtlW = chance.substatWeightTotal - goodTtlW;
        if(hasDown){//chance of parent get good
            for(int pid = 0; pid < graph.three.combination[idx-1].len; pid++){
                STATE *parent = graph.three.combination[idx-1].ptr + pid;
                STAT *pStat = (STAT*)parent->arg;
                
                int goodW = goodTtlW;
                int restW = chance.substatWeightTotal;
                for(int argId = 0; argId < idx-1; argId++){
                    goodW -= chance.substatWeight[ pStat[argId] ];  
                    restW -= chance.substatWeight[ pStat[argId] ];  
                }
                for(int argId = idx-1; argId < 3; argId++){
                    restW -= chance.substatWeight[ pStat[argId] ];  
                }
                
                double pC = 1.f*goodW/restW; 
                node->parentChance[pCid++] = pC;
                totalChance += parent->parentChance[0]*pC;

                // /*  */printf("good[%d].parentChance[%d,%d]=%lf",idx,idx-1,pid)
    
            }
        }
        if(hasEqual){//chance of parent get bad
            for(int pid = 0; pid < graph.three.combination[idx].len; pid++){
                STATE *parent = graph.three.combination[idx].ptr + pid;
                
                int badW = badTtlW;
                int restW = chance.substatWeightTotal;
                for(int argId = 2; argId >= idx; argId--){
                    badW  -= chance.substatWeight[ ((STAT*)parent->arg)[argId] ];
                    restW -= chance.substatWeight[ ((STAT*)parent->arg)[argId] ];
                }
                for(int argId = idx-1; argId >= 0; argId--){
                    restW -= chance.substatWeight[ ((STAT*)parent->arg)[argId] ];
                }
                
                double pC = 1.f*badW/restW; 
                node->parentChance[pCid++] = pC;
                totalChance += parent->parentChance[0]*pC;
    
            }
        }
        
        node->heapNode = NULL;
        sprintf(node->msg,"Good3-%d:(%.6lf)",idx,totalChance);
        addtoChecksum;

    }


    //deallocate combination.arg
    for(int idx = 0; idx<4; idx++){
        STATE *layer = graph.three.combination[idx].ptr;
        for(int i = 0; i<graph.three.combination[idx].len; i++){
            free(layer[i].arg);
            layer[i].arg = NULL;
        }

    }
    #ifdef graph_init_good_3_log
        #undef addtoChecksum 
        printf("graph.3.good initialized; from start, sum = %.6lf\n",checksum);
    #endif 
    #undef graph_init_good_3_log
}
void graph_init_good_4(){
    
    
    
    for(int idx=0; idx<graph.four.good.len; idx++){
        STATE *node = graph.four.good.ptr + idx;

        node->price = cost.upgrade[0];
        node->successChance = 0;
        sprintf(node->msg,"Good4-%d",idx);
        node->whitelisted = 0;
        
        node->arg = NULL;
        node->child = graph.four.upgrade[idx].ptr;
        node->childCount = graph.four.upgrade[idx].len?2:0;//the first 2 node
        
        
        node->parentCount = 1;
        node->parent = graph.four.start.ptr;
        node->parentChance = (double *)malloc(sizeof(double));
        node->parentChance[0] = 0;

        

        int i1, i2, i3, i4; 
        int i1Len,i2Len,i3Len,i4Len;// (good/bad)Len
        STAT *src1,*src2,*src3,*src4;// (good/bad)Arr
        int nol=-1,*i2p = &i1,*i3p = &i2,*i4p = &i3;
        switch(idx){
            #define assign_to_(q1,q2,q3,q4)\
            i1Len = chance.substat##q1##Count;\
            src1 = chance.substat##q1;\
            i2Len = chance.substat##q2##Count;\
            src2 = chance.substat##q2;\
            i3Len = chance.substat##q3##Count;\
            src3 = chance.substat##q3;\
            i4Len = chance.substat##q4##Count;\
            src4 = chance.substat##q4;

            case 0:assign_to_(Bad ,Bad ,Bad ,Bad );break;
            case 1:assign_to_(Good,Bad ,Bad ,Bad );i2p = &nol;break;
            case 2:assign_to_(Good,Good,Bad ,Bad );i3p = &nol;break;
            case 3:assign_to_(Good,Good,Good,Bad );i4p = &nol;break;
            case 4:assign_to_(Good,Good,Good,Good);break;
            default:assert(0);

            #undef assign_to_
        }

        
        
        for(i1 = 0; i1<i1Len; i1++){
        for(i2 = *i2p + 1; i2<i2Len; i2++){
        for(i3 = *i3p + 1; i3<i3Len; i3++){
        for(i4 = *i4p + 1; i4<i4Len; i4++){
            int subsW[4] = {
                 chance.substatWeight[src1[i1]]
                ,chance.substatWeight[src2[i2]]
                ,chance.substatWeight[src3[i3]]
                ,chance.substatWeight[src4[i4]]
            };


            
            double denomSum = 0;
            int denom1 = chance.substatWeightTotal;
            for(int c1 = 0; c1<4; c1++){
                int denom2 = denom1 - subsW[c1];
            for(int c2 = 0; c2<4; c2++){
                if(c2==c1)continue;
                int denom3 = denom2 - subsW[c2];
            for(int c3 = 0; c3<4; c3++){
                if(c3==c1)continue;
                if(c3==c2)continue;
                int denom4 = denom3 - subsW[c3];
            // for(int c4 = 0; c4<4; c4++){//only 1                
                denomSum += 1.f/denom1/denom2/denom3/denom4;
            }
            }
            }
            int nom = subsW[0] * subsW[1] * subsW[2] * subsW[3];
            node->parentChance[0] += nom * denomSum;
            
            
        }
        }
        }
        }
        
        node->heapNode = NULL;

    }


    #ifdef graph_init_good_4_log
        double checksum = 0.f;
        for(int i = 0; i<graph.four.good.len; i++) {
            double pchance = graph.four.good.ptr[i].parentChance[0];
            checksum+=pchance;
            // printf("graph.4.good[%d]:%lf",i,pchance);
        }
            
        printf("graph.4.good initialized; sum of pC = %lf\n",checksum);
    #endif 
    #undef graph_init_good_4_log
}
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

    STATE *curLayer = upgrade;
    for(int l = 2; l<=maxLayer ; l++){
        char parentIsGood = (l==2)
            ,lastLayer = (l==maxLayer);

        int curCost = cost.upgrade[4 - (maxLayer-l)];
        STATE *parentLayer = curLayer - (l-1);
        STATE *childLayer = curLayer + l;

        for(int up = 0; up<l; up++){
            STATE *curNode = curLayer + up;

            curNode->price = curCost;
            curNode->successChance = 
                lastLayer*(index  + up>=threshold);
            sprintf(curNode->msg,"up%d-%d,%d",branch,index,up);
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

    #ifdef debug_init_upgrade_log
        printf("graph.%d.upgrade[%d] initialized\n",branch,index);
    #endif
    #undef debug_init_upgrade_log
}


#ifdef graph_init_good_4_log
        double checksum = 0.f;
        for(int i = 0; i<graph.four.good.len; i++) {
            double pchance = graph.four.good.ptr[i].parentChance[0];
            checksum+=pchance;
            // printf("graph.4.good[%d]:%lf",i,pchance);
        }
            
        printf("graph.4.good initialized; sum of pC = %lf\n",checksum);
    #endif 
    #undef graph_init_good_4_log
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
        // json_printGraph(HEAP_file,graph.heap->root,(JSON_PRINT_FUNC *)__json_printSpanningTree_HEAP_NODE,"created");
        printf("graph initialized\n");
    }

    
#endif



//deallocate all the memory allocated inside graph
void graph_close(){
    HEAP_close(&graph.heap);

    if(!graph.root.ptr){
        printf("graph state root empty\n");
        return;
    }
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

    #ifdef debug_close_log
        printf("graph closed\n");
    #endif
    #undef debug_close_log
}

/* ---follow up propagation still untested */
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
        
    }

    return rootIsModified;
}

//return whether graph.root is modified
int graph_propagate_peak(){
    int rootIsModified = 0;

    //pop peak
    STATE *peakNode = HEAP_pop(graph.heap);
    peakNode->heapNode = NULL;
    peakNode->whitelisted = STATUS_ACCEPTED;
    
    
    double peakPrice = peakNode->price;
    double peakRate = peakNode->successChance;
    rootIsModified += graph_propagate_from(peakNode,peakPrice,peakRate);
  
    
    #ifdef debug_propagate_peak
        char tmp[100];
        sprintf(tmp,"propagated %s",peakNode->msg);
        #ifdef debug_propagate_peak_log 
            printf("%s\n",tmp);
        #endif 
        #undef debug_propagate_peak_log

        #ifdef debug_propagate_peak_print 
            __printHeap;
            __printGraph;
        #endif 
        #undef debug_propagate_peak_print
    #endif 
    #undef debug_propagate_peak

    
    return rootIsModified;
}


//for heap, is the relation that must be kept between (parent,child)
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
    fprintf(outFile,",\"price\":%.6lf"  ,node->price);
    fprintf(outFile,",\"succesR\":%.6lf",node->successChance);
    fprintf(outFile,",\"accept\":%d"    ,node->whitelisted);
    fprintf(outFile,",\"inHeap\":%d"    ,node->heapNode != NULL);
    fprintf(outFile,"}");
}
void STATE_fprintLinkD(FILE *outFile, STATE *parentNode, STATE *childNode){
    if(!parentNode ){
        fprintf(outFile,"{\"chance\":0}");
        return;
    }
    #ifdef debug
        if(childNode->parentChance == NULL) {
            fprintf(
                outFile
                ,"{\"chance\":\"TEMP\"}"
            );
            return;
        }
    #endif
    fprintf(
        outFile,
        "{\"chance\":%.6lf}"
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
    fprintf(outFile,",\"succesR\":%.8lf",node_data->successChance);
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