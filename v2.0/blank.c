
typedef struct{
    void *arg;
}STATE;
STATE *STATE_getChild(STATE *node, int index);
int   STATE_getChildCount(STATE *node);
void  STATE_fprintNodeD(FILE *outFile,STATE *node);
void  STATE_fprintLinkD(FILE *outFile, STATE *parentNode, STATE *childNode);
void  STATE_clearArg();

/* 
NODE{
void* arg;
have all function
}
*/


void __json_printSpanningTree(void* _root, FILE *treeF, FILE *outerF, int *nodeCount, int *outer_exist){
    #define NODE STATE
    #define NODE_getChild STATE_getChild
    #define NODE_getChildCount STATE_getChildCount
    #define NODE_fprintNodeD STATE_fprintNodeD
    #define NODE_fprintLinkD STATE_fprintLinkD
    #define NODE_clearArg STATE_clearArg

    //result
    *outer_exist = 0;
    *nodeCount = 0;
      
    //for bfs
    NODE *root = (NODE *)_root;
    typedef struct QUEUE {
        NODE *node;
        struct QUEUE *next;
    } QUEUE;
    QUEUE *queueTemp = NULL, *queueRoot = NULL, *queueTail = NULL;
    NODE *theNode;
    
    
    //symbollically placing root on queue
    root->arg = malloc(sizeof(int));
    *(int *)root->arg = (*nodeCount)++;

    fprintf(treeF,"            {");
    fprintf(treeF," \"id\":%d",*(int *)root->arg);
    fprintf(treeF,",\"parentId\":\"\"");
    fprintf(treeF,",\"linkData\":{\"chance\":100}");
    fprintf(treeF,",\"nodeData\":");NODE_fprintNodeD(treeF,root);
    fprintf(treeF,"},\n");
    //pick root as current link
    theNode = root;

    while(1){
        int theNodeChildCount = NODE_getChildCount(theNode);
        for(int i = 0; i < theNodeChildCount; i++){
            NODE *child = NODE_getChild(theNode,i);

            if(!child->arg){//if it has never been in queue,

                //give id
                child->arg = malloc(sizeof(int));
                *(int *)child->arg = (*nodeCount)++;

                //add to queue
                if(queueTail)  queueTail = queueTail->next = malloc(sizeof(QUEUE));
                else  queueTail = queueRoot = malloc(sizeof(QUEUE));
                queueTail->node = child;
                queueTail->next = NULL;
                

                //print link(this,child) in treelink along with its data, 
                fprintf(treeF,"            {");
                fprintf(treeF," \"id\":%d",*(int *)child->arg);
                fprintf(treeF,",\"parentId\":%d",*(int *)theNode->arg);
                fprintf(treeF,",\"linkData\":");
                NODE_fprintLinkD(treeF,theNode,child);
                fprintf(treeF,",\"nodeData\":");
                NODE_fprintNodeD(treeF,child);
                fprintf(treeF,"},\n");

            } else {//else print this link in outer
                *outer_exist = 1;
                fprintf(outerF,"            {");
                fprintf(outerF," \"id\":%d",*(int *)child->arg);
                fprintf(outerF,",\"parentId\":%d",*(int *)theNode->arg);
                fprintf(outerF,",\"linkData\":");
                NODE_fprintLinkD(outerF,theNode,child);
                fprintf(outerF,"},\n");
            }

            
        }
        
        
        //pick current node
        if(queueRoot){
            theNode = queueRoot->node;
            if(queueRoot->next){
                queueTemp = queueRoot;
                queueRoot = queueRoot->next;
                free(queueTemp);
            }else{
                free(queueRoot);
                queueTail = queueRoot = NULL;
            }

        } else break;

    }


    //clean up
    NODE_clearArg();
    #undef NODE
    #undef NODE_getChild
    #undef NODE_getChildCount
    #undef NODE_fprintNodeD
    #undef NODE_fprintLinkD
    #undef NODE_clearArg
}
