#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"relic_globalConstant.h"
#include"relic_JSON.h"
#include"relic_STATE.h"
#include"relic_HEAP.h"

/* 
note

heap link has the detail of the parent (to test the parent property)


*/


int main(){
    JSON *file = json_init("heapData.json");
   
    STATE a,b,c,d,e;
    a.successChance = 1;
    b.successChance = 2;
    c.successChance = 3;
    d.successChance = 4;
    e.successChance = 5;

    a.price = 100;
    b.price = 100;
    c.price = 100;
    d.price = 100;
    e.price = 100;

    a.whitelisted = 0;
    b.whitelisted = 0;
    c.whitelisted = 0;
    d.whitelisted = 0;
    e.whitelisted = 0;

    sprintf(a.msg,"a");
    sprintf(b.msg,"b");
    sprintf(c.msg,"c");
    sprintf(d.msg,"d");
    sprintf(e.msg,"e");
   
    

    //added e
    HEAP *root = HEAP_init(&e);
    //json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"added e");
    
    //added b
    HEAP *Hb =HEAP_add(root,&b,(Compare *)isMoreEfficient);
    //json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"added b");
    
    //added a
    HEAP *Ha =HEAP_add(root,&a,(Compare *)isMoreEfficient);
    //json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"added a");
    
    //added d
    HEAP *Hd =HEAP_add(root,&d,(Compare *)isMoreEfficient);
    //json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"added d");

    //added c
    HEAP *Hc =HEAP_add(root,&c,(Compare *)isMoreEfficient);
    // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"added c");
    json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"created");
    
    //swap a and c
    HEAP_swap(&(root->right),HEAP_right,&(root->left->right),HEAP_right);
    json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"swap a and c");

    //swap a and e
    HEAP_swap(&(root->left->right),HEAP_right,&(root),HEAP_none);
    json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"swap a and e");
    
    

    // //normalize d
    // HEAP_normalizeDown(root->left->left,(int (*)(void *, void *))isMoreEfficient);
    //// json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"normalize d");
    
    // //modifiy e
    // e.successChance = 0.1f;
    //// json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"modifiy e");

    // //normalize a
    // HEAP_normalizeDown(root,(int (*)(void *, void *))isMoreEfficient);
    //// json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"normalize a");
    

    HEAP_free(&root);
    
    
    
    



 


    

    json_close(file);

    printf("\ndone\n");
    return 0;
}



