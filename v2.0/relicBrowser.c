#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"relic_globalConstant.h"
#include"relic_JSON.h"
#include"relic_STATE.h"
#include"relic_HEAP.h"






//for min HEAP
typedef struct HEAP_STATE{
    int length;
    // void *data;
    /*  */int data;
    struct HEAP_STATE *left;
    struct HEAP_STATE *right;

    //for printing
    void * arg;
} HEAP_STATE;
/* 
return binary representation. Exp: 6 = 1,1,0
reminder: free out_binary
*/
void int2binary(int number, char **out_binary, int *out_length){
    if(number == 0){
        *out_length = 1;
        *out_binary = calloc(1,sizeof(char));
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
HEAP_STATE* HEAP_STATE_init(STATE * data){
    HEAP_STATE *result = calloc(1,sizeof(HEAP_STATE));
    result->length = 1;
    result->data = data;
    return result;
}
void HEAP_STATE_free(HEAP_STATE * root);
void int2binary(int number, char **out_binary, int *out_length);
void HEAP_STATE_add(HEAP_STATE * root, STATE * data);
HEAP_STATE* HEAP_STATE_pop(HEAP_STATE * root);


//for printing
HEAP_STATE *HEAP_STATE_getChild(HEAP_STATE *node, int index){return index? node->right: node->left;}
int HEAP_STATE_getChildCount(HEAP_STATE *node){return (node->left != NULL) + (node->right != NULL);}
/* test */void HEAP_STATE_fprintNodeD(FILE *outFile,HEAP_STATE *node){
    //STATE_fprintNodeD(outFile,node->data);
    fprintf(outFile,"{%d}",node->data);
}
void HEAP_STATE_fprintLinkD(FILE *outFile, HEAP_STATE *parentNode, HEAP_STATE *childNode){fprintf(outFile,"{}");}
void HEAP_STATE_clearArg(HEAP_STATE *root){
    free(root->arg);
    root->arg = NULL;
    if(root->left)HEAP_STATE_clearArg(root->left);
    if(root->right)HEAP_STATE_clearArg(root->right);
}



//for JSON








int main(){
    // // initialize constant variable
    // STAT substat[] = {STAT_CD,STAT_ATKP};
    // iniGlobalVariable(PIECE_BODY,STAT_CR,substat,sizeof(substat)/sizeof(int)); 
    // struct COST cost2;
    // struct CHANCE chance2;
    // memcpy(&cost2,&cost,sizeof(struct COST));
    // memcpy(&chance2,&chance,sizeof(struct CHANCE));

    
    //drawing a STATE graph
    graph_init_test();
    JSON *mainGraphJson = json_init("mydata.json");
    json_printGraph(
        mainGraphJson,
        (void *)graph.root.ptr,
        (void (*)(void *, FILE *, FILE *, int *, int *))__json_printSpanningTree_STATE
    );
    json_close(mainGraphJson);
    

    
    

    

    
    printf("\ndone\n");
    return 0;
}










