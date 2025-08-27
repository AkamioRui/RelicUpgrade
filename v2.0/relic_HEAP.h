#ifndef relic_HEAP
#define relic_HEAP

#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<string.h>
#include<assert.h>
#include "relic_JSON.h"


#define HEAP_DATA void 

//debugging 
// #define debugging
#ifdef debugging
    #define debugging_init
    #define debugging_close 
    /* js */#define debugging_closeBranch
    /* js */#define debugging_add 
    // /* js */#define debugging_normalizeDown
    // /* js */#define debugging_normalizeUp
    // /* js */#define debugging_swap
    /* js */#define debugging_HEAP_pop

    #define HEAP_NODE_print(node){\
        printf(#node":{");\
        if(node) printf(" id:%s \tparent:%s \tleft:%s \trigth:%s ",\
            node->data?node->data->msg:"nD"\
            ,node->parent ? \
                (node->parent->data?node->parent->data->msg:"nD"): "_"\
            ,node->left ? \
                (node->left->data?node->left->data->msg:"nD"): "_"\
            ,node->right ? \
                (node->right->data?node->right->data->msg:"nD"): "_"\
        );\
        printf("}\n");\
    }

    JSON *HEAP_file;
    typedef struct {
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
        struct HEAP_NODE *heapNode;
    
    } HEAP_DATA_TEMP;
    #undef HEAP_DATA
    #define HEAP_DATA HEAP_DATA_TEMP
#endif

  

typedef int(HEAP_Compare)(void *parent, void*child); 
typedef struct HEAP_NODE{
    struct HEAP_NODE *left;
    struct HEAP_NODE *right;
    struct HEAP_NODE *parent;

    HEAP_DATA* data;// if debugging is turned on, its STATE
    

    //for printing
    void * arg;
}HEAP_NODE;
typedef struct HEAP{
    int length;
    HEAP_NODE *root;
    HEAP_Compare *cmp;
} HEAP;
typedef enum HEAP_SIDE {
    HEAP_none, //&root and root relationship
    HEAP_left, //child is on the left of parent
    HEAP_right //child is on the right of parent
}HEAP_SIDE;





//for printing, 
//for testing purposes, this currently has dependency with relic_STATE
HEAP_NODE *HEAP_NODE_getChild(HEAP_NODE *node, int index){
    return index? node->right: node->left;
}
int HEAP_NODE_getChildCount(HEAP_NODE *node){
    return 2;//print will iterate this many time, I set it to 2 such that it will itereate over left and right regardless if its empty
    // return (node->left != NULL) + (node->right != NULL);
}
void HEAP_NODE_fprintNodeD(FILE *outFile,HEAP_NODE *node);// its in relic_STATE.h
void HEAP_NODE_fprintLinkD(FILE *outFile, HEAP_NODE *parentNode, HEAP_NODE *childNode);// its in relic_STATE.h
void HEAP_NODE_clearArg(HEAP_NODE *root){
    typedef struct STACK {
        HEAP_NODE *param;
        int position; //0: done none, 1: done left, 2: done right 
        struct STACK *next;
    } STACK;
    STACK *baseStack = NULL, *tmpStack = NULL;

    //recursion parameter
    baseStack = (STACK *)malloc(sizeof(STACK));
    baseStack->position = 0;
    baseStack->param = root;
    baseStack->next = NULL;

    

    //loop
    do{
        
        HEAP_NODE *theNode = baseStack->param;
        switch(baseStack->position){
            case 0://if(root->left)HEAP_clearArg(root->left);
                if(theNode->left){
                    
                    
                    
                    //add to stack
                    baseStack->position = 1;
                    tmpStack = (STACK *)malloc(sizeof(STACK));
                    tmpStack->param = theNode->left;
                    tmpStack->position = 0;
                    tmpStack->next = baseStack;
                    baseStack = tmpStack;
                    tmpStack = NULL;
                    

                    
                    
                    break;
                }
                
            case 1://if(root->right)HEAP_clearArg(root->right);
                if(theNode->right){
                    
    
                    //add to stack
                    baseStack->position = 2;
                    tmpStack = (STACK *)malloc(sizeof(STACK));
                    tmpStack->param = theNode->right;
                    tmpStack->position = 0;
                    tmpStack->next = baseStack;
                    baseStack = tmpStack;
                    tmpStack = NULL;
                    

                    
                    break;
                }
    
            case 2://free(theNode->arg); theNode->arg = NULL; return;
                
                
                free(theNode->arg);
                theNode->arg = NULL;
    
                //pop stack
                tmpStack = baseStack;
                baseStack = baseStack->next;
                free(tmpStack);
                tmpStack = NULL;
                
            break;
        }
    } while(baseStack);
    





}
__json_printSpannigTree_generic(HEAP_NODE)




//utility
HEAP* HEAP_init();
void HEAP_close(HEAP **heapPtr);
void HEAP_closeBranch(HEAP *heap, HEAP_NODE *node);
HEAP_NODE *HEAP_add(HEAP *heap, void * data);
void *HEAP_pop(HEAP *heap);

void int2binary(int number, char **out_binary, int *out_length);
HEAP_NODE **HEAP_pointer_to(HEAP *heap, int index);
void HEAP_swap(HEAP *heap , HEAP_NODE *node1, HEAP_NODE *node2);
void HEAP_normalizeUp(HEAP *heap,HEAP_NODE *node);
void HEAP_normalizeDown(HEAP *heap ,HEAP_NODE *node);
void HEAP_normalize(HEAP *heap ,HEAP_NODE *node);






// return binary representation. Exp: 6 = 1,1,0
// reminder: free out_binary
void int2binary(int number, char **out_binary, int *out_length){
    //assert
    if(number == 0){
        *out_length = 1;
        *out_binary = (char *)calloc(1,sizeof(char));
        return;
    }

    //finding length
    int length = 0;
    for(int _number = number; _number > 0 ; _number >>= 1) length++;

    //fill char[length] with 1 and 0
    char *binary = (char *)malloc(length*sizeof(char));
    for(int _number = number, idx = length-1; idx >= 0; idx--){
        binary[idx] = _number % 2;
        _number >>= 1;
    }

    //the return statement
    *out_length = length;
    *out_binary = binary;
}

//return ptr pointing to the left/right element inside the parent that point to the actual node
HEAP_NODE **HEAP_pointer_to(HEAP *heap, int index){
    //if it request for index = 0, ie the root
    assert(index >= 0);
    HEAP_NODE **rootPtr = &(heap->root);
    
    //get path
    index++;
    int pathLen;
    char *path;//1 = left, 0 = rigth
    int2binary(index,&path,&pathLen);

    //navigate to new node
    HEAP_NODE **newNodePtr = rootPtr;
    for(int i = 1; i<pathLen; i++){
        newNodePtr = path[i]? &(*newNodePtr)->right : &(*newNodePtr)->left; 
    }

    //free path
    free(path);
    path = NULL;
    return newNodePtr;

}

//initialize a heap with 0 length and root = NULL
HEAP* HEAP_init(HEAP_Compare *cmp){
    #ifdef debugging_init
        printf("HEAP initialized\n");
    #endif
    HEAP *result = (HEAP *)calloc(1,sizeof(HEAP));
    result->length = 0;
    result->root = NULL;
    result->cmp = cmp;
    return result;
}

//delete the HEAP and all of its HEAP_NODE, also set HEAP = NULL
void HEAP_close(HEAP **heapPtr){
    #define __close_printLength
    #ifdef debugging_close
        int __close__length = (*heapPtr)->length;
        #define __close_printLength printf("delete heap with length %d\n",__close__length);
    #endif
    
    
    //actual code
    
    HEAP *heap = *heapPtr;
    
    if((heap)->root) HEAP_closeBranch(heap,(heap)->root);
    free(heap);
    *heapPtr = NULL;
    __close_printLength;

    //macro clean up
    #undef __close_printLength

}
// memory pointed by node and its children is deallocated
void HEAP_closeBranch(HEAP *heap, HEAP_NODE *node){
    //for testing
    #define __closeBranch__testprint 
    #define __closeBranch__printStack
    #define __closeBranch__printDeleted
    #define __closeBranch__printAddedtoStack
    #ifdef debugging_closeBranch
        char tmp[1024];
        sprintf(tmp,"deleted %s",node->data->msg);
        // #define __closeBranch__testprint json_printGraph(HEAP_file,heap->root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP_NODE,tmp);
        // #define __closeBranch__printStack printf("stack:");for(CONTEXT *i = context; i; i=i->next)printf("[%s]",(*i->param)->data->msg);
        #define __closeBranch__printDeleted printf("deleted %s\n",(*newNodePtr)->data->msg);
        // #define __closeBranch__printAddedtoStack(nodePtr) printf("added %s\n",(nodePtr)->data->msg);

    #endif
    
    //find rootPtr
    HEAP_NODE **rootPtr;
    if(heap->root != node){//node is not root
        rootPtr = node->parent->left == node? 
            &(node->parent->left): 
            &(node->parent->right);
    } else { //node is root
        rootPtr = &(heap->root);
    }

    
    
    

    typedef struct CONTEXT{
        struct CONTEXT *next;
        int position;
        HEAP_NODE **param;

    }CONTEXT;
    CONTEXT *context, *tmpContext;
    

    //create first context
    context = (CONTEXT *)malloc(sizeof(CONTEXT));
    context->next = NULL;
    context->param = rootPtr;
    context->position = 0;


    while(context){
        __closeBranch__printStack;

        //recover parameter 
        HEAP_NODE **newNodePtr = context->param;

        //recover position
        switch(context->position){
            case 0://if((*rootPtr)->left)HEAP_free((*rootPtr)->left);
                if((*newNodePtr)->left){
                    __closeBranch__printAddedtoStack((*newNodePtr)->left);
                    //context management
                    context->position = 1;
                    tmpContext = (CONTEXT *)malloc(sizeof(CONTEXT));
                    tmpContext->next = context;
                    tmpContext->param = &(*newNodePtr)->left;
                    tmpContext->position = 0;
                    context = tmpContext;
                    tmpContext = NULL;
                    break;
                }


            case 1://if((*rootPtr)->right)HEAP_free((*rootPtr)->right);
                if((*newNodePtr)->right){
                    __closeBranch__printAddedtoStack((*newNodePtr)->right);
                    //context management
                    context->position = 2;
                    tmpContext = (CONTEXT *)malloc(sizeof(CONTEXT));
                    tmpContext->param = &(*newNodePtr)->right;
                    tmpContext->position = 0;
                    tmpContext->next = context;
                    context = tmpContext;
                    tmpContext = NULL;
                    break;
                }
            case 2:
                __closeBranch__printDeleted;
                free(*newNodePtr);
                *newNodePtr = NULL;

                //delete context
                tmpContext = context;
                context = context->next;
                free(tmpContext);
                tmpContext = NULL;
            break;
            default: printf("error occured"); break;
        }
        
    }

    
    
    __closeBranch__testprint;
    #undef __closeBranch__testprint
    #undef __closeBranch__printStack
    #undef __closeBranch__printDeleted
    #undef __closeBranch__printAddedtoStack

}

//going from the left to the right, layer by layer, add a node 
//while preserving cmp(parent,child)
//return new node
HEAP_NODE *HEAP_add(HEAP *heap, void * data){
    #define __add_printAdded
    #ifdef debugging_add
        char tmp[100]; 
        
        #define __add_printAdded\
        sprintf(tmp,"created %s",newNode->data?newNode->data->msg:"_");\
        // json_printGraph(HEAP_file,heap->root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP_NODE,tmp);\
        printf("%s\n",tmp);\

    #endif
     
    HEAP_Compare *cmp = heap->cmp;

    //creating the node
    int newNodeIdx = (heap->length)++;
    HEAP_NODE **newNodePtr = HEAP_pointer_to(heap, newNodeIdx);
    HEAP_NODE *newNode = *newNodePtr = (HEAP_NODE *)calloc(1,sizeof(HEAP_NODE));

    //fill-in node data
    
    newNode->data = data;
    if(newNodeIdx){
        //if the node has odd index, then its on the left
        newNode->parent = (HEAP_NODE *)((char *)newNodePtr 
        - (newNodeIdx % 2 ? offsetof(HEAP_NODE,left):offsetof(HEAP_NODE,right))
        );
    } else { //is root
        newNode->parent = NULL;
    }
    

    // validate result
    HEAP_normalizeUp(heap,newNode);



    /* test */
    __add_printAdded;
    return newNode;
    #undef __add_printAdded 

}

//return the top node's pointer to data, and delete that node;
void *HEAP_pop(HEAP *heap){
    #define __pop__printHeap
    #ifdef debugging_HEAP_pop
        char tmp[1000];
        sprintf(tmp,"poped %s",heap->root->data->msg);
        printf("%s\n",tmp);
        // #define __pop__printHeap json_printGraph(HEAP_file,heap->root,(JSON_PRINT_FUNC *)__json_printSpanningTree_HEAP_NODE,tmp);
    #endif

    //special case
    if(heap->length == 0) return NULL;
    if(heap->length == 1){
        void *peakData = heap->root->data;assert(peakData);
        free(heap->root);
        heap->root = NULL;
        return peakData;
    }


    //some constant
    void *peakData = heap->root->data;
    HEAP_Compare *cmp = heap->cmp;

    //swap peakNode with lastNode
    HEAP_NODE *peakNode = heap->root;
    HEAP_NODE *lastNode = *HEAP_pointer_to(heap,heap->length - 1);
    HEAP_swap(heap,peakNode,lastNode);

    //delete the new lastNode (previously the peakNode)
    *HEAP_pointer_to(heap,heap->length - 1) = NULL;
    free(peakNode);peakNode = NULL;
    heap->length--;
    
    //normalize down the new peakNode (previously the lastNode)
    HEAP_normalizeDown(heap,lastNode);

    __pop__printHeap;
    return peakData;
    #undef __pop__printHeap
}

//perform a deep swap, what is modified is the left,right,parent. the data is never moved 
void HEAP_swap(HEAP *heap , HEAP_NODE *node1, HEAP_NODE *node2){
    #define __swap__printHeap
    #ifdef debugging_swap
        char tmp[1024];
        sprintf(tmp,"swap %s, %s",node1->data->msg,node2->data->msg);
        printf("%s\n",tmp);
        // #define __swap__printHeap json_printGraph(HEAP_file,heap->root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP_NODE,tmp);
    #endif
    
    assert(heap);
    assert(node1);
    assert(node2);

    //based on node1, node2, determine their pointer
    HEAP_NODE **rootPtr = &(heap->root);
    HEAP_NODE **node1_ptr, **node2_ptr;
    node1_ptr = node1->parent?
        (node1->parent->left == node1? 
            &(node1->parent->left)
            : &(node1->parent->right)
        )
        : rootPtr;
    node2_ptr = node2->parent?
        (node2->parent->left == node2? 
            &(node2->parent->left)
            : &(node2->parent->right)
        )
        : rootPtr;

    // if(node2->parent){
    //     if(node2->parent->left == node2){
    //         node2_ptr = &(node2->parent->left);
            
    //     } else {
    //         node2_ptr = &(node2->parent->right);
            
    //     }
    // } else {
    //     node2_ptr = rootPtr;
        
    // }
    

    


    //assume node 1 & node 2 are not directly linked
    HEAP_NODE *node1_parent = node1->parent;
    HEAP_NODE *node2_parent = node2->parent;
    HEAP_NODE *node1_left = node1->left;
    HEAP_NODE *node1_right = node1->right;
    HEAP_NODE *node2_left = node2->left;
    HEAP_NODE *node2_right = node2->right;

    int nodePair_code = ((node1->left  == node2)<<3)
                       +((node1->right == node2)<<2)
                       +((node2->left  == node1)<<1)
                       +((node2->right == node1))
    ;

    //node 1 environment
    *node1_ptr = node2;
    if(node1_left)node1_left->parent = node2;
    if(node1_right)node1_right->parent = node2;

    //node 2 environment
    *node2_ptr = node1;
    if(node2_left)node2_left->parent = node1;
    if(node2_right)node2_right->parent = node1;

    //node 1 itself
    node1->parent = node2_parent;
    node1->left = node2_left;
    node1->right = node2_right;

    //node 2 itself
    node2->parent = node1_parent;
    node2->left = node1_left;
    node2->right = node1_right;

    //fixing the issue caused by both node linked together
    //it will only affect property of those 2 node
    switch(nodePair_code){
        case 0b0000: break;
        case 0b1000: // before swap, node1.left = node2
            node1->parent = node2;
            node2->left = node1;
        break;
        case 0b0100: // before swap, node1.right = node2
            node1->parent = node2;
            node2->right = node1;
        break;
        case 0b0010: // before swap, node2.left = node1
            node1->left = node2;
            node2->parent = node1;
        break;  
        case 0b0001: // before swap, node2.right = node1
            node1->right = node2;
            node2->parent = node1;
        break;
        default: 
            assert(nodePair_code); 
        return;
    }

    //macro clean up
    __swap__printHeap;
    #undef __swap__printHeap

}

//starting at node going up, making sure cmp(parent,child) is true 
void HEAP_normalizeUp(HEAP *heap,HEAP_NODE *node){
    #define __normalizeUp__printHeap
    #ifdef debugging_normalizeUp
        char tmp[1024];
        sprintf(tmp,"normalizeUp %s",node->data->msg);
        printf("%s\n",tmp);
        // #define __normalizeUp__printHeap json_printGraph(HEAP_file,heap->root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP_NODE,tmp);
    #endif
    HEAP_Compare *cmp = heap->cmp;

    while(node->parent){
        if(cmp(node->parent->data,node->data)) break;
        HEAP_swap(heap,node->parent,node);
    }
    

    __normalizeUp__printHeap;
    #undef __normalizeUp__printHeap
}   

//starting at node going down, making sure cmp(parent,child) is true
void HEAP_normalizeDown(HEAP *heap ,HEAP_NODE *node){
    #define __normalizeDown__printHeap
    #ifdef debugging_normalizeDown
        char tmp[1024];
        sprintf(tmp,"normalizeDown %s",node->data->msg);
        printf("%s\n",tmp);
        // #define __normalizeDown__printHeap json_printGraph(HEAP_file,heap->root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP_NODE,tmp);
    #endif
    HEAP_Compare *cmp = heap->cmp;


    HEAP_NODE *current = node;
    while(1){

        HEAP_NODE *candidate_child;
        switch (2*(current->left != 0) + (current->right !=0 )){
            case 0b00:
                __normalizeDown__printHeap;
                return;
            break;
            case 0b01://error
                printf("HEAP_normalizeDown: left child empty, but right is not");
                return;
                // candidate_child = current->right;
            break;
            case 0b10:
                candidate_child = current->left;
            break;
            case 0b11:
                if(cmp(current->left->data,current->right->data)){
                    candidate_child = current->left;
                } else {
                    candidate_child = current->right;
                }
            break;
            default:
                assert("invalid code");
            break;
        }

        
        if(cmp(current->data,candidate_child->data))break;
        HEAP_swap(heap,current,candidate_child);   
    }  
    
    
    __normalizeDown__printHeap;
    #undef __normalizeDown__printHeap
 }
 



#undef debugging
#endif

