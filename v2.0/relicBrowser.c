#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"relic_globalConstant.h"
#include"relic_JSON.h"
#include"relic_STATE.h"
#include"relic_HEAP.h"




int main(){

   
    STATE a,b,c,d,e;
    a.successChance = .1f;
    b.successChance = .2f;
    c.successChance = .3f;
    d.successChance = .4f;
    e.successChance = .5f;

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





    

    

   
    JSON *file = json_init("heapData.json");

    HEAP *root = HEAP_init(&e);
    json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP);

    HEAP_add(root,&b,(int (*)(void *, void *))moreEfficient);
    json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP);

    HEAP_add(root,&a,(int (*)(void *, void *))moreEfficient);
    json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP);

    HEAP_add(root,&d,(int (*)(void *, void *))moreEfficient);
    json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP);

    HEAP_add(root,&c,(int (*)(void *, void *))moreEfficient);
    json_printGraph(file,root,(void (*)(void*, FILE *, FILE *, int *, int *))__json_printSpanningTree_HEAP);
    
    
    
    
    
    json_close(file);



 


    

    
    printf("\ndone\n");
    return 0;
}



