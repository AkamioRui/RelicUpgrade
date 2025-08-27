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
    // HEAP_file = json_init("result/HEAP.json");
    // STATE_file = json_init("result/STATE.json");

    STAT substat[] = {STAT_CD,STAT_ATKP,STAT_SPD};
    initGlobalVariable(
        PIECE_BODY
        ,STAT_CR
        ,substat
        ,sizeof(substat)/sizeof(STAT)
        ,6
    );
    
    struct COST cost2;
    struct CHANCE chance2;
    memcpy(&cost2,&cost,sizeof(struct COST));
    memcpy(&chance2,&chance,sizeof(struct CHANCE));

    /* partial */
    //initialize the graph; 
    // graph_init_test_pyramid4(sizeof(substat)/sizeof(STAT));
    // while(1){
    //     if(graph.heap->root == NULL){
    //         printf("!!!heap is NULL\n");//ment all node is included, if not then there is some elimination
    //         break;
    //     }
    //     if(isMoreEfficient(graph.root.ptr,graph.heap->root->data)) break;
    //     graph_propagate_peak();
    // }
   
    



    

    // graph_close();
    // json_close(STATE_file);
    // json_close(HEAP_file);
    printf("\ndone\n");
    return 0;
}
/* 
    // FILE *log = fopen("log.c","w");
    // HEAP_NODE_print(graph.heap->root);
    // graph_printState(log);
    // fclose(log);
*/
/* 
        ],
        "outerLinks":[

        ]
    } 
]
*/
/* 
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(gdb) Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": "${fileDirname}/${fileBasenameNoExtension}.exe",
            "args": [],
            "cwd": "${fileDirname}",
            "environment": [],
            "stopAtEntry": false,
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "C:/MinGW/bin/gdb.exe",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                },
                {
                    "description": "Set Disassembly Flavor to Intel",
                    "text": "-gdb-set disassembly-flavor intel",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "C/C++: gcc.exe build active file"
        }


    ]
}

*/

