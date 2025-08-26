#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"relic_JSON.h"
#include"relic_HEAP.h"
#include"relic_STATE.h"

int main(){
    HEAP_file = json_init("heapData.json");
   
    
    //  {price, successChance, msg[16], whitelisted}
    STATE a={100,1,"a",0};
    STATE b={100,2,"b",0};
    STATE c={100,3,"c",0};
    STATE d={100,4,"d",0};
    STATE e={100,5,"e",0};
   

    //creating 
    HEAP *theHeap = HEAP_init((HEAP_Compare *)isMoreEfficient);
    HEAP_NODE *He =HEAP_add(theHeap,&e);
    HEAP_NODE *Hb =HEAP_add(theHeap,&b);
    HEAP_NODE *Ha =HEAP_add(theHeap,&a);
    HEAP_NODE *Hd =HEAP_add(theHeap,&d);
    HEAP_NODE *Hc =HEAP_add(theHeap,&c);
    json_printGraph(HEAP_file,theHeap->root,(JSON_PRINT_FUNC *)__json_printSpanningTree_HEAP_NODE,"created");

    
    
    // //normalize test
    // HEAP_swap(theHeap,He,Ha);
    // HEAP_swap(theHeap,He,Hb);
    // json_printGraph(HEAP_file,theHeap->root,(JSON_PRINT_FUNC *)__json_printSpanningTree_HEAP_NODE,"swap");
    // HEAP_normalizeDown(theHeap,Ha);
    
    //pop test
    HEAP_pop(theHeap);



    HEAP_close(&theHeap);


    json_close(HEAP_file);
    printf("\ndone\n");
    return 0;
}
/* 
        ],
        "outerLinks":[

        ]
    } 
]
*/

