#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"relic_globalConstant.h"
#include"relic_JSON.h"
#include"relic_STATE.h"
#include"relic_HEAP.h"

/* 
note
1. relic STATE add print node to console

*/

/* 
high level operation
initialize the graph;
add all node that has steap > 0 into heap;
while (heap){
    until root is modified{
        pop the steapest from heap, and whitelist it (its data is forwarded to all of its parent)

        //print the graph
        //print the heap
    }
    remove all node less steap than the root, blacklist them

    //print the graph
    //print the heap
}
print the graph (whitelisted = green, blackListed = red, unknown = red);

*/

int main(){
    HEAP_file = json_init("HEAP.json");
    STATE_file = json_init("STATE.json");

    STAT substat[] = {STAT_CD};
    initGlobalVariable(PIECE_BODY,STAT_CR,substat,sizeof(substat)/sizeof(STAT),3);
    graph_init_test_pyramid4(sizeof(substat)/sizeof(STAT));
    // FILE *log = fopen("log.c","w");
    // graph_printState(log);
    // fclose(log);

    json_printGraph(STATE_file,graph.root.ptr,(JSON_PRINT_FUNC *)__json_printSpanningTree_STATE,"created");


    json_close(HEAP_file);
    json_close(STATE_file);
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


