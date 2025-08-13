#ifndef relic_HEAP
#define relic_HEAP

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "relic_JSON.h"

//for min HEAP
typedef struct HEAP{
    int length;
    void *data;//for now it is int
    struct HEAP *left;
    struct HEAP *right;

    //for printing
    void * arg;
} HEAP;

//for printing
HEAP *HEAP_getChild(HEAP *node, int index){return index? node->right: node->left;}
int HEAP_getChildCount(HEAP *node){return (node->left != NULL) + (node->right != NULL);}
void HEAP_fprintNodeD(FILE *outFile,HEAP *_node){
    STATE * node = _node->data;

    fprintf(outFile,"{");
    fprintf(outFile," \"detail\":\"%s\"",node->msg);
    fprintf(outFile,",\"price\":%.2lf"  ,node->price);
    fprintf(outFile,",\"succesR\":%.2lf",node->successChance);
    fprintf(outFile,",\"accept\":%d"    ,node->whitelisted);
    fprintf(outFile,"}");
}
void HEAP_fprintLinkD(FILE *outFile, HEAP *parentNode, HEAP *childNode){fprintf(outFile,"{}");}
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
    HEAP *nextNode;
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
    if(number == 0){
        *out_length = 1;
        *out_binary = (char *)calloc(1,sizeof(char));
        return;
    }

    int length = 0;
    for(int _number = number; _number > 0 ; _number >>= 1) length++;

    char *binary = (char *)malloc(length*sizeof(char));
    for(int _number = number, idx = length-1; idx >= 0; idx--){
        binary[idx] = _number % 2;
        _number >>= 1;
    }

    *out_length = length;
    *out_binary = binary;
}
HEAP* HEAP_init(void * data){
    HEAP *result = (HEAP *)calloc(1,sizeof(HEAP));
    result->length = 1;
    result->data = data;
    return result;
}
void HEAP_free(HEAP * root);
void HEAP_add(HEAP * root, void * data, int (*cmp)(void *, void*)){
    //index changed from starting at 0 to 1
    int newnodeID = ++(root->length);


    
    typedef struct STACK{
        HEAP *heap;
        struct STACK *next;//basically parent
    }STACK;
    STACK *baseStack = NULL, *tmpStack;
    // #define printStack() for(STACK *ptr = baseStack; ptr; ptr = ptr->next) printf("[%d] ",*(int *)ptr->heap->data);printf("\n");

    //get path
    int pathLen;
    char *path;//1 = left, 0 = rigth
    int2binary(newnodeID,&path,&pathLen);

    //create path stack
    HEAP *heapPtr = root;
    baseStack = (STACK *)malloc(sizeof(STACK));
    baseStack->next = NULL;
    baseStack->heap = heapPtr;
    for(int i = 1; i<pathLen-1; i++){
        heapPtr = path[i]? heapPtr->right : heapPtr->left ;

        
        tmpStack = (STACK *)malloc(sizeof(STACK));
        tmpStack->heap = heapPtr;
        tmpStack->next = baseStack;
        baseStack = tmpStack;
        tmpStack = NULL;
        
    }
    if(path[pathLen-1] == 0)heapPtr = heapPtr->left = (HEAP *)calloc(1,sizeof(HEAP));
    else heapPtr = heapPtr->right = (HEAP *)calloc(1,sizeof(HEAP));
    heapPtr->data = data;
    free(path);
    path = NULL;


    //from newly added node, make sure the heap is valid
    void *tmpdata;
    while(baseStack){
        if(cmp((void*)baseStack->heap->data,(void*)heapPtr->data)) break;
        tmpdata = baseStack->heap->data ;
        baseStack->heap->data = heapPtr->data;
        heapPtr->data = tmpdata;

        heapPtr = baseStack->heap;
        tmpStack = baseStack;
        baseStack = baseStack->next;
        free(tmpStack);
        tmpStack = NULL;
    }
    
    // JSON *file = json_init("heapData.json");
    // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP);
    // json_close(file);

    
    
    
    return;
    #undef printStack()
}
HEAP* HEAP_pop(HEAP * root);




    
   
    
    
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