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
    file = json_init("heapData.json");
   
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
    HEAP *theHeap = HEAP_init();
    HEAP_NODE *He =HEAP_add(theHeap,&e,(HEAP_Compare *)isMoreEfficient);
    HEAP_NODE *Hb =HEAP_add(theHeap,&b,(HEAP_Compare *)isMoreEfficient);
    HEAP_NODE *Ha =HEAP_add(theHeap,&a,(HEAP_Compare *)isMoreEfficient);
    HEAP_NODE *Hd =HEAP_add(theHeap,&d,(HEAP_Compare *)isMoreEfficient);
    HEAP_NODE *Hc =HEAP_add(theHeap,&c,(HEAP_Compare *)isMoreEfficient);

    // HEAP *He = HEAP_add();
    // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"added e");
    
    //added b
    // HEAP *Hb =HEAP_add(&root,&b,(HEAP_Compare *)isMoreEfficient);
    // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"added b");
    
    //added a
    // HEAP *Ha =HEAP_add(&root,&a,(HEAP_Compare *)isMoreEfficient);
    // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"added a");
    
    //added d
    // HEAP *Hd =HEAP_add(&root,&d,(HEAP_Compare *)isMoreEfficient);
    // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"added d");

    //added c
    // HEAP *Hc =HEAP_add(&root,&c,(HEAP_Compare *)isMoreEfficient);
    // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"added c");
    // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"created");

    //pop
    // HEAP_pop(&root);
    // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"pop");

    // //normalize e
    // e.successChance = 0.5f;
    // HEAP_normalizeDown(&root,He,(int (*)(void *, void *))isMoreEfficient);
    // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"normalize e=0.5");

    // //normalize a
    // HEAP_normalizeDown(&root,Ha,(int (*)(void *, void *))isMoreEfficient);
    // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"normalize a");

    // //normalize d
    // a.successChance = 10;
    // HEAP_normalizeDown(&root,Hd,(int (*)(void *, void *))isMoreEfficient);
    // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"normalize b");
    
    // //swap d and b
    // HEAP_swap(&root,Hd,Hb);
    // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"swap d and b");

    // //swap d and a
    // HEAP_swap(&root,Hd,Ha);
    // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"swap a and e");

    // //swap e and a
    // HEAP_swap(&root,root,Ha);
    // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"swap a and e");
    
    

    
    
    // //modifiy e
    // e.successChance = 0.1f;
    // // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"modifiy e");

    // //normalize a
    // HEAP_normalizeDown(root,(int (*)(void *, void *))isMoreEfficient);
    // // json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP,"normalize a");
    

    // HEAP_free(&root);
    
    
    
    



 


    

    json_close(file);
/* 
        ],
        "outerLinks":[

        ]
    } 
]
*/


    printf("\ndone\n");
    return 0;
}



