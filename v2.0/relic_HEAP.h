#ifndef elic_HEAP
#define elic_HEAP

#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<string.h>
#include<assert.h>
#include "relic_JSON.h"

/** note to self:
 * 1. finish HEAP_pop
 * 2. generalize this to support any TYPE of data
 * 
 * 
 *  */ 

/*  *///debugging
JSON *file;
   

typedef struct HEAP_NODE{
    struct HEAP_NODE *left;
    struct HEAP_NODE *right;
    struct HEAP_NODE *parent;

    // void *data;//for now it is int
    /*  */struct {
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
    
    }* data;
    

    //for printing
    void * arg;
}HEAP_NODE;
typedef struct HEAP{
    int length;
    HEAP_NODE *root;
} HEAP;

//none is to descript &root and root relationship
typedef enum HEAP_SIDE {HEAP_none, HEAP_left, HEAP_right}HEAP_SIDE;
typedef int(HEAP_Compare)(void *parent, void*child); 




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
void int2binary(int number, char **out_binary, int *out_length);
HEAP_NODE **HEAP_pointer_to(HEAP *heap, int index);
HEAP* HEAP_init();
void HEAP_close(HEAP **heapPtr);
void HEAP_closeBranch(HEAP *heap, HEAP_NODE *node);

HEAP_NODE *HEAP_add(HEAP *heap, void * data, HEAP_Compare *cmp);
///* WIP */void HEAP_pop(HEAP_NODE *root);
///* WIP */void HEAP_normalizeUp(HEAP_NODE **rootPtr,HEAP_NODE *node,HEAP_Compare *cmp);
///* WIP */void HEAP_normalizeDown(HEAP_NODE **rootPtr,HEAP_NODE *node,HEAP_Compare *cmp);


/* 
return binary representation. Exp: 6 = 1,1,0
reminder: free out_binary
*/
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
HEAP* HEAP_init(){
    HEAP *result = (HEAP *)calloc(1,sizeof(HEAP));
    result->length = 0;
    result->root = NULL;

}
/* ---no normalize */
HEAP_NODE *HEAP_add(HEAP *heap, void * data, HEAP_Compare *cmp){
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
    // HEAP_normalizeUp(rootPtr,newNode,cmp);



    /* test */
    char tmp[100];
    assert(newNode->data);
    sprintf(tmp,"created %s",newNode->data->msg);
    json_printGraph(file,heap->root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP_NODE,tmp);
    return newNode;

}


void HEAP_close(HEAP **heapPtr){
    #define __printLength
    #define __printLength printf("delete heap with length %d\n",(*heapPtr)->length);
    
    //actual code
    __printLength;
    HEAP *heap = *heapPtr;
    HEAP_closeBranch(heap,(heap)->root);
    free(heap);
    *heapPtr = NULL;

    //macro clean up
    #undef __printLength

}
// memory pointed by node and its children is deallocated
void HEAP_closeBranch(HEAP *heap, HEAP_NODE *node){
    //for testing
    #define __testprint 
    #define __printStack
    #define __printDeleted
    #define __printAdded

    // char tmp[1024];
    // sprintf(tmp,"deleted %s",node->data->msg);
    // #define __testprint json_printGraph(file,heap->root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP_NODE,tmp);
    // #define __printStack printf("stack:");for(CONTEXT *i = context; i; i=i->next)printf("[%s]",(*i->param)->data->msg);
    #define __printDeleted printf("deleted %s\n",(*newNodePtr)->data->msg);
    // #define __printAdded(nodePtr) printf("added %s\n",(nodePtr)->data->msg);
    
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
        __printStack;

        //recover parameter 
        HEAP_NODE **newNodePtr = context->param;

        //recover position
        switch(context->position){
            case 0://if((*rootPtr)->left)HEAP_free((*rootPtr)->left);
                if((*newNodePtr)->left){
                    __printAdded((*newNodePtr)->left);
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
                    __printAdded((*newNodePtr)->right);
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
                __printDeleted;
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

    
    
    __testprint;
    #undef __testprint
    #undef __printStack
    #undef __printDeleted
    #undef __printAdded

}

// //return ptr pointing to the left/right element inside the parent that point to the actual node
// HEAP_NODE **HEAP_pointer_to(HEAP_NODE **rootPtr, int index){
//     //if it request for index = 0, ie the root
//     if(index < 0)return rootPtr;
    
//     //get path
//     index++;
//     int pathLen;
//     char *path;//1 = left, 0 = rigth
//     int2binary(index,&path,&pathLen);

//     //navigate to new node
//     HEAP_NODE **newNodePtr = rootPtr;
//     for(int i = 1; i<pathLen; i++){
//         newNodePtr = path[i]? &(*newNodePtr)->right : &(*newNodePtr)->left; 
//     }

//     //free path
//     free(path);
//     path = NULL;
//     return newNodePtr;

// }


// //input must be either &root(not a copy of the root) or &(heap.left/right)
// void HEAP_swap(HEAP_NODE **rootPtr, HEAP_NODE *node1, HEAP_NODE *node2){
//     assert(rootPtr);
//     assert(node1);
//     assert(node2);

//     //based on node1, node2, determine their pointer
//     HEAP_NODE **node1_ptr, **node2_ptr;
//     HEAP_SIDE node1_side, node2_side;
//     if(node1->parent){
//         if(node1->parent->left == node1){
//             node1_ptr = &(node1->parent->left);
//             node1_side = HEAP_left;
//         } else {
//             node1_ptr = &(node1->parent->right);
//             node1_side = HEAP_right;
//         }
//     } else {
//         node1_ptr = rootPtr;
//         node1_side = HEAP_none;
//     }
//     if(node2->parent){
//         if(node2->parent->left == node2){
//             node2_ptr = &(node2->parent->left);
//             node2_side = HEAP_left;
//         } else {
//             node2_ptr = &(node2->parent->right);
//             node2_side = HEAP_right;
//         }
//     } else {
//         node2_ptr = rootPtr;
//         node2_side = HEAP_none;
//     }
    

    


//     //assume node 1 & node 2 are not directly linked
//     HEAP_NODE *node1_parent = node1->parent;
//     HEAP_NODE *node2_parent = node2->parent;
//     HEAP_NODE *node1_left = node1->left;
//     HEAP_NODE *node1_right = node1->right;
//     HEAP_NODE *node2_left = node2->left;
//     HEAP_NODE *node2_right = node2->right;

//     int nodePair_code = ((node1->left  == node2)<<3)
//                        +((node1->right == node2)<<2)
//                        +((node2->left  == node1)<<1)
//                        +((node2->right == node1))
//     ;

//     //node 1 environment
//     *node1_ptr = node2;
//     if(node1_left)node1_left->parent = node2;
//     if(node1_right)node1_right->parent = node2;

//     //node 2 environment
//     *node2_ptr = node1;
//     if(node2_left)node2_left->parent = node1;
//     if(node2_right)node2_right->parent = node1;

//     //node 1 itself
//     node1->parent = node2_parent;
//     node1->left = node2_left;
//     node1->right = node2_right;

//     //node 2 itself
//     node2->parent = node1_parent;
//     node2->left = node1_left;
//     node2->right = node1_right;

//     //fixing the issue caused by both node linked together
//     //it will only affect property of those 2 node
//     switch(nodePair_code){
//         case 0b0000: break;
//         case 0b1000: // before swap, node1.left = node2
//             node1->parent = node2;
//             node2->left = node1;
//         break;
//         case 0b0100: // before swap, node1.right = node2
//             node1->parent = node2;
//             node2->right = node1;
//         break;
//         case 0b0010: // before swap, node2.left = node1
//             node1->left = node2;
//             node2->parent = node1;
//         break;  
//         case 0b0001: // before swap, node2.right = node1
//             node1->right = node2;
//             node2->parent = node1;
//         break;
//         default: 
//             assert(nodePair_code); 
//         return;
//     }

// }

// //starting at node going up, making sure cmp(parent,child) is true 
// void HEAP_normalizeUp(HEAP_NODE **rootPtr,HEAP_NODE *node,HEAP_Compare *cmp){
//     while(node->parent){
//         if(cmp(node->parent->data,node->data)) break;
//         HEAP_swap(rootPtr,node->parent,node);
//     }
// }   

// //starting at node going down, making sure cmp(parent,child) is true
// void HEAP_normalizeDown(HEAP_NODE *rootPtr ,HEAP_NODE *node,HEAP_Compare *cmp){

//     HEAP_NODE *current = node;
//     while(1){

//         HEAP_NODE *candidate_child;
//         switch (2*(current->left != 0) + (current->right !=0 )){
//             case 0b00:
//                 return;
//             break;
//             case 0b01:
//                 assert("left child empty, but right is not");
//                 return;
//                 // candidate_child = current->right;
//             break;
//             case 0b10:
//                 candidate_child = current->left;
//             break;
//             case 0b11:
//                 if(cmp(current->left->data,current->right->data)){
//                     candidate_child = current->left;
//                 } else {
//                     candidate_child = current->right;
//                 }
//             break;
//             default:
//                 assert("invalid code");
//             break;
//         }


//         if(cmp(current->data,candidate_child->data))return;
//         HEAP_swap(rootPtr,current,candidate_child);
        



        

        
//     }

    
    
//  }
 
// //append new node, while preserving cmp(parent,child)
// //return new node




// //pop the root node,  return that data
//// /* WIP */void* HEAP_pop(HEAP_NODE **rootPtr){
//     /*  */printf("root has %d node;\n",(*rootPtr)->length);
//     HEAP_NODE *first = *rootPtr;//point to the top most node in heap
//     void *theData = (*rootPtr)->data;

//     //swap root to the highest index node
//     HEAP_NODE *last = *HEAP_pointer_to(rootPtr,(*rootPtr)->length - 1);
//     HEAP_SIDE lastSide = 
//         last->parent->left == last? HEAP_left: HEAP_right;
//     HEAP_swap(rootPtr,first,last);


//     //free root thats now is at the highest index node
    
//     free(first);
//     first = NULL;
    
    

//     //HEAP_normalizeDown(highest index node)

    
//     //is this allowed?
//     json_printGraph(file,*rootPtr,(void (*)(void*, FILE *, FILE *, int *, int *))_json_printSpanningTree_HEAP,"swap with last");

    


//     return theData;
// }




    
//debug
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
    
    
/* 
    int 
    a = 0, 
    b = 1, 
    c = 2, 
    d = 3, 
    e = 4, 
    f = 5, 
    g = 6
    ;
    
    HEAP_NODE *root_a = HEAP_init(&a);
    HEAP_NODE *node_b = (HEAP_NODE *)calloc(1,sizeof(HEAP_NODE));
    HEAP_NODE *node_c = (HEAP_NODE *)calloc(1,sizeof(HEAP_NODE));
    HEAP_NODE *node_d = (HEAP_NODE *)calloc(1,sizeof(HEAP_NODE));
    HEAP_NODE *node_e = (HEAP_NODE *)calloc(1,sizeof(HEAP_NODE));
    HEAP_NODE *node_f = (HEAP_NODE *)calloc(1,sizeof(HEAP_NODE));
    HEAP_NODE *node_g = (HEAP_NODE *)calloc(1,sizeof(HEAP_NODE));

    node_b->data = &b;
    node_c->data = &c;
    node_d->data = &d;
    node_e->data = &e;
    node_f->data = &f;
    node_g->data = &g;
   
    root_a->left = node_b;
    root_a->right = node_c;
    node_b->left = node_d;
    node_b->right = node_e;
    node_c->left = node_f;
    node_c->right = node_g;

    root_a->length = 7;


    int h = 2;
    HEAP_add(root_a,&h,lessThan);

    free(root_a);
    free(node_b);
    free(node_c);
    free(node_d);
    free(node_e);
    free(node_f);
    free(node_g);

*/
#endif