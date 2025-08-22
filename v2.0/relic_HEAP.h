#ifndef relic_HEAP
#define relic_HEAP

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


typedef struct HEAP{
    int length;
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
        struct HEAP *heap;
    
    } * data;
    struct HEAP *left;
    struct HEAP *right;
    struct HEAP *parent;

    //for printing
    void * arg;
} HEAP;

//none is to descript &root and root relationship
typedef enum HEAP_SIDE {HEAP_none, HEAP_left, HEAP_right}HEAP_SIDE;

typedef int(HEAP_Compare)(void *parent, void*child); 




//for printing, 
//for testing purposes, this currently has dependency with relic_STATE
HEAP *HEAP_getChild(HEAP *node, int index){return index? node->right: node->left;}
int HEAP_getChildCount(HEAP *node){return (node->left != NULL) + (node->right != NULL);}
void HEAP_fprintNodeD(FILE *outFile,HEAP *node);// its in relic_STATE.h
void HEAP_fprintLinkD(FILE *outFile, HEAP *parentNode, HEAP *childNode);// its in relic_STATE.h
void HEAP_clearArg(HEAP *root){
    typedef struct STACK {
        HEAP *param;
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
        
        HEAP *theNode = baseStack->param;
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
__json_printSpannigTree_generic(HEAP)


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
HEAP* HEAP_init(void * data){
    HEAP *result = (HEAP *)calloc(1,sizeof(HEAP));
    result->length = 1;
    result->data = data;
    result->parent = NULL;
    return result;
}
void HEAP_free(HEAP ** root){
    typedef struct CONTEXT{
        struct CONTEXT *next;
        int position;
        HEAP **param;

    }CONTEXT;
    CONTEXT *context, *tmpContext;
    // #define printStack() printf("stack:");for(CONTEXT *i = context; i; i=i->next)printf("[%.2lf]",((STATE *)(*i->param)->data)->successChance);
    
    
    
    //create first context
    context = (CONTEXT *)malloc(sizeof(CONTEXT));
    context->next = NULL;
    context->param = root;
    context->position = 0;


    while(context){
        //recover parameter 
        HEAP **heapPtr = context->param;
        
        

        //recover position
        switch(context->position){
            case 0://if((*root)->left)HEAP_free((*root)->left);
                if((*heapPtr)->left){
                    //context management
                    context->position = 1;
                    tmpContext = (CONTEXT *)malloc(sizeof(CONTEXT));
                    tmpContext->next = context;
                    tmpContext->param = &(*heapPtr)->left;
                    tmpContext->position = 0;
                    context = tmpContext;
                    tmpContext = NULL;
                    break;
                }


            case 1://if((*root)->right)HEAP_free((*root)->right);
                if((*heapPtr)->right){
                    //context management
                    context->position = 2;
                    tmpContext = (CONTEXT *)malloc(sizeof(CONTEXT));
                    tmpContext->param = &(*heapPtr)->right;
                    tmpContext->position = 0;
                    tmpContext->next = context;
                    context = tmpContext;
                    tmpContext = NULL;
                    break;
                }
            case 2:
                free(*heapPtr);
                *heapPtr = NULL;

                //delete context
                tmpContext = context;
                context = context->next;
                free(tmpContext);
                tmpContext = NULL;
            break;
            default: printf("error occured"); break;
        }
        

    }

    // #undef printStack
}
//return ptr pointing to the left/right element inside the parent that point to the actual node
HEAP **HEAP_pointer_to(HEAP **rootPtr, int index){
    //if it request for index = 0, ie the root
    if(index < 0)return rootPtr;
    
    //get path
    index++;
    int pathLen;
    char *path;//1 = left, 0 = rigth
    int2binary(index,&path,&pathLen);

    //navigate to new node
    HEAP **heapPtr = rootPtr;
    for(int i = 1; i<pathLen; i++){
        heapPtr = path[i]? &(*heapPtr)->right : &(*heapPtr)->left; 
    }

    //free path
    free(path);
    path = NULL;
    return heapPtr;

}


//input must be either &root(not a copy of the root) or &(heap.left/right)
void HEAP_swap(HEAP **rootPtr, HEAP *node1, HEAP *node2){
    assert(rootPtr);
    assert(node1);
    assert(node2);

    //based on node1, node2, determine their pointer
    HEAP **node1_ptr, **node2_ptr;
    HEAP_SIDE node1_side, node2_side;
    if(node1->parent){
        if(node1->parent->left == node1){
            node1_ptr = &(node1->parent->left);
            node1_side = HEAP_left;
        } else {
            node1_ptr = &(node1->parent->right);
            node1_side = HEAP_right;
        }
    } else {
        node1_ptr = rootPtr;
        node1_side = HEAP_none;
    }
    if(node2->parent){
        if(node2->parent->left == node2){
            node2_ptr = &(node2->parent->left);
            node2_side = HEAP_left;
        } else {
            node2_ptr = &(node2->parent->right);
            node2_side = HEAP_right;
        }
    } else {
        node2_ptr = rootPtr;
        node2_side = HEAP_none;
    }
    

    


    //assume node 1 & node 2 are not directly linked
    HEAP *node1_parent = node1->parent;
    HEAP *node2_parent = node2->parent;
    HEAP *node1_left = node1->left;
    HEAP *node1_right = node1->right;
    HEAP *node2_left = node2->left;
    HEAP *node2_right = node2->right;

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

}

//starting at node going up, making sure cmp(parent,child) is true 
void HEAP_normalizeUp(HEAP **rootPtr,HEAP *node,HEAP_Compare *cmp){
    while(node->parent){
        if(cmp(node->parent->data,node->data)) break;
        HEAP_swap(rootPtr,node->parent,node);
    }
}   

//starting at node going down, making sure cmp(parent,child) is true
void HEAP_normalizeDown(HEAP *rootPtr ,HEAP *node,HEAP_Compare *cmp){

    HEAP *current = node;
    while(1){

        HEAP *candidate_child;
        switch (2*(current->left != 0) + (current->right !=0 )){
            case 0b00:
                return;
            break;
            case 0b01:
                assert("left child empty, but right is not");
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


        if(cmp(current->data,candidate_child->data))return;
        HEAP_swap(rootPtr,current,candidate_child);
        



        

        
    }

    
    
 }
 
//append new node, while preserving cmp(parent,child)
//return new node
HEAP *HEAP_add(HEAP **rootPtr, void * data, HEAP_Compare *cmp){

    HEAP *root = *rootPtr;
    HEAP **heapPtr = HEAP_pointer_to(rootPtr, (root->length)++);
    HEAP *newNode = *heapPtr = (HEAP *)calloc(1,sizeof(HEAP));
    int side = root->length%2;
    //root.length = root.length +1. because I need index + 1
    newNode->parent = (HEAP *)((char *)heapPtr 
    - (side?offsetof(HEAP,right):offsetof(HEAP,left))
    );
    newNode->data = data;

    HEAP_normalizeUp(rootPtr,newNode,cmp);//still problematic
    return newNode;

}



//pop the root node,  return that data
/* WIP */void* HEAP_pop(HEAP **rootPtr){
    /*  */printf("root has %d node;\n",(*rootPtr)->length);
    HEAP *first = *rootPtr;//point to the top most node in heap
    void *theData = (*rootPtr)->data;

    //swap root to the highest index node
    HEAP *last = *HEAP_pointer_to(rootPtr,(*rootPtr)->length - 1);
    HEAP_SIDE lastSide = 
        last->parent->left == last? HEAP_left: HEAP_right;
    HEAP_swap(rootPtr,first,last);
    

    //free root thats now is at the highest index node
    
    free(first);
    first = NULL;
    
    

    //HEAP_normalizeDown(highest index node)

    
    //is this allowed?
    json_printGraph(file,*rootPtr,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"swap with last");

    


    return theData;
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
    
    HEAP *root_a = HEAP_init(&a);
    HEAP *node_b = (HEAP *)calloc(1,sizeof(HEAP));
    HEAP *node_c = (HEAP *)calloc(1,sizeof(HEAP));
    HEAP *node_d = (HEAP *)calloc(1,sizeof(HEAP));
    HEAP *node_e = (HEAP *)calloc(1,sizeof(HEAP));
    HEAP *node_f = (HEAP *)calloc(1,sizeof(HEAP));
    HEAP *node_g = (HEAP *)calloc(1,sizeof(HEAP));

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