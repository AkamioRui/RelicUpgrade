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
    STATE aa;
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
   
    

    HEAP *theHeap = HEAP_init();
    HEAP_NODE *He =HEAP_add(theHeap,&e,(HEAP_Compare *)isMoreEfficient);
    HEAP_NODE *Hb =HEAP_add(theHeap,&b,(HEAP_Compare *)isMoreEfficient);
    HEAP_NODE *Ha =HEAP_add(theHeap,&a,(HEAP_Compare *)isMoreEfficient);
    HEAP_NODE *Hd =HEAP_add(theHeap,&d,(HEAP_Compare *)isMoreEfficient);
    HEAP_NODE *Hc =HEAP_add(theHeap,&c,(HEAP_Compare *)isMoreEfficient);

    
    HEAP_swap(theHeap,He,Ha);
    HEAP_normalizeUp(theHeap,He,isMoreEfficient);
    HEAP_close(&theHeap);


    

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



