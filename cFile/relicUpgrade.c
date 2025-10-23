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
#define min_heap_check
#define printAt 
#ifdef debug
#define min_heap_check
    double efficientcy = 0;/* rate/cost */\
    for(int nodeId = 0; nodeId<graph.root.len; nodeId++){\
        STATE *node = graph.root.ptr + nodeId;\
        if(node->whitelisted)continue;\
        double curScore = node->price>0?\
        node->successChance/node->price:\
        0;\
        efficientcy = efficientcy>curScore?efficientcy:curScore;\
    }\
    double heapEfficientcy = graph.heap->root->data->successChance / graph.heap->root->data->price;\
    if(efficientcy != heapEfficientcy)printf("%4d; heap inefficient (%lf,%lf) \n",i,efficientcy,heapEfficientcy);


#define printAt\
    switch(i){\
        case 139:\
        case 140:\
        json_printGraph(HEAP_file,graph.heap->root,(JSON_PRINT_FUNC *)__json_printSpanningTree_HEAP_NODE,"");\
        json_printGraph(STATE_file,graph.root.ptr,(JSON_PRINT_FUNC *)__json_printSpanningTree_STATE,"");\
    }

#endif 

void printGuide(){
    printf("argument syntax: piece mainstat threshold sub...(atleast 1)\n");
    printf("argument must be using numbers/code:\n");
    
    printf(
        "piece: \n"
        "HEAD   :0\n"
        "HANDS  :1\n"
        "BODY   :2\n"
        "BOOTS  :3\n"
        "PLANAR :4\n"
        "ROPE   :5\n\n"
        "stat: \n"
        "HP     : 0\n"
        "ATK    : 1\n"
        "DEF    : 2\n"
        "HPP    : 3\n"
        "ATKP   : 4\n"
        "DEFP   : 5\n"
        "SPD    : 6\n"
        "CR     : 7\n"
        "CD     : 8\n"
        "EHR    : 9\n"
        "ERS    : 10\n"
        "BE     : 11\n"
        "DMG    : -1\n"
        "HEAL   : -2\n"
        "ER     : -3\n"       
    );    
}
void createLABEL(
    PIECE piece, 
    STAT mainstat, 
    int threshold, 
    int substatC, 
    STAT *substat
){
    FILE* lableFile = fopen("../result/LABEL.json","w");
        fprintf(lableFile,"{\n");
        fprintf(lableFile,"   \"piece\":%d,\n",piece);
        fprintf(lableFile,"   \"mainstat\":%d,\n",mainstat);
        fprintf(lableFile,"   \"threshold\":%d,\n",threshold);
        fprintf(lableFile,"   \"substat\":[");
        for(int i = 0; i<substatC; i++){
            if(i != 0)fprintf(lableFile,", %d",substat[i]);
            else      fprintf(lableFile,"%d",substat[i]);
        }
        fprintf(lableFile,"]\n");
        fprintf(lableFile,"}");
    fclose(lableFile);
}


// ./browserUI/relicUpgrade 2 7 5 8 4
//gdb --args ./relicUpgrade 2 7 5 8 4
int main(int argc, char **argv){
    //relicUpgrade piece mainstat threshold sub1 sub2 ...
    
    PIECE piece ;
    STAT mainstat ;
    int threshold ;
    int substatC = argc - 4;
    STAT substat[12];
    
    if(argc < 5){
        printf("too few argument\n");
        printGuide();
        return 1;
    }
    

    if(sscanf(argv[1], "%d", &piece    )<1){
        printf("piece must be a code\n");
        printGuide();
        return 1;
    }
    if(sscanf(argv[2], "%d", &mainstat )<1){
        printf("mainstat must be a code\n");
        printGuide();
        return 1;
    }
    if(sscanf(argv[3], "%d", &threshold)<1){
        printf("threshold must be a code\n");
        printGuide();
        return 1;
    }
    for(int i = 0; i<substatC; i++) {
        if(sscanf(argv[i+4],"%d",substat+i)<1){
            printf("all substat must be a code\n");
            printGuide();
            return 1;
        }
    }

    // createLABEL(piece, mainstat, threshold, substatC, substat);


    // PIECE piece = 2;
    // STAT mainstat = 7;
    // int threshold = 5;
    // int substatC = 2;
    // STAT substat[12] = {8,4,0};
    


    initGlobalVariable(
        piece
        ,mainstat
        ,substat
        ,substatC
        ,threshold
    );
    // struct CHANCE *chance2 = &chance;


    
    // HEAP_file = json_init("result/HEAP.json");
    STATE_file = json_init("../result/STATE.json");
    graph_init();
    // struct GRAPH *graph2 = &graph;




    for(int i = 0;; i++){
        if(graph.heap->root == NULL){
            printf("heap is NULL\n");//ment all node is included, if not then there is some elimination
            break;
        }
        if(isMoreEfficient(graph.root.ptr,graph.heap->root->data)) break;
        min_heap_check;

        graph_propagate_peak();
        printAt;
    }
    // json_printGraph(HEAP_file,graph.heap->root,(JSON_PRINT_FUNC *)__json_printSpanningTree_HEAP_NODE,"");
    json_printGraph(STATE_file,graph.root.ptr,(JSON_PRINT_FUNC *)__json_printSpanningTree_STATE,"");
    
    



    graph_close();
    json_close(STATE_file);
    // json_close(HEAP_file);
    
    printf("\ndone\n");
    return 0;
}
#undef debug
#undef min_heap_check 
#undef printAt


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

