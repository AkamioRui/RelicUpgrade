
/** also assign Id */
/* need to also print spanning tree links  */void __json_printGraphNodes(JSON *json){
    fprintf(json->outFile,"        \"nodes\":[\n");
    

    typedef struct QUEUE {
        int depth;
        STATE *node;
        struct QUEUE *next;
    } QUEUE;
    QUEUE *queueHead = NULL, *_queue, *queueTail = NULL;
    memset(json->queued,0,graph.root.len); 
   

    STATE *current = graph.root.ptr;
    short currentNodeId = 0;
    short currentDepth = 0;
    while(1){
        //print current /* this should print: node data, the spanning link, all links */
        fprintf(json->outFile,"           ");
        if(currentNodeId)fprintf(json->outFile,",");
        fprintf(json->outFile,"{\"depth\":%d,\"price\":%lf,\"succesR\":%lf,\"detail\":\"%s\",\"accept\":\"%d\"}\n"
            ,currentDepth
            ,current->price
            ,current->successChance
            ,current->msg
            ,(int)current->whitelisted
        );
        
        
        //append children into queue(if not already in queue)
        currentDepth++;
        for(int i = 0; i<current->childCount; i++){
            if( json->queued[current->child + i - graph.root.ptr] ) continue;
            //create queue node
            _queue = (QUEUE *)malloc(sizeof(QUEUE));
            _queue->node = current->child + i;
            _queue->depth = currentDepth;
            _queue->next = NULL;

            //append queue node
            if(queueTail == NULL){ 
                queueHead = queueTail = _queue;
            } else {
                queueTail->next = _queue;
                queueTail = queueTail->next;
            }
            _queue = NULL;

            //mark node by assigning id
            currentNodeId++;
            json->queued[current->child + i - graph.root.ptr] = currentNodeId;

        }
        
        //pop QUEUE, set to current
        if(queueHead == NULL)break;        
        current = queueHead->node;
        currentDepth = queueHead->depth;

        if(queueHead == queueTail){
            free(queueHead);
            queueHead = queueTail = NULL;
        } else {
            _queue = queueHead;
            queueHead = queueHead->next;
            free(_queue);
            _queue = NULL;
        }

    } 

    fprintf(json->outFile,"        ],\n");
}
/* this will be redundant */void __json_printGraphLinks(JSON *json){
    fprintf(json->outFile,"        \"links\":[\n");
    

    typedef struct QUEUE {
        STATE *node;
        struct QUEUE *next;
    } QUEUE;
    QUEUE *queueHead = NULL, *queueTail = NULL, *_queue;
   

    STATE *current = graph.root.ptr;
    short nodeid = 0;
    while(1){
        
        //print current links
        STATE *currentParent;
        for(int i = 0; i<current->parentCount; i++){
            currentParent = current->parent + i;

            fprintf(json->outFile,"           ");
            if(nodeid) fprintf(json->outFile,",");
            fprintf(json->outFile,"{\"src\":%d,\"dst\":%d,\"chance\":%lf}\n"
                ,abs(json->queued[currentParent - graph.root.ptr])
                ,abs(json->queued[current - graph.root.ptr])  
                ,current->parentChance[i]
            );
            nodeid++;
        }

        //append children into queue(if not already in queue)
        STATE *currentChild;
        for(int i = 0; i<current->childCount; i++){
            currentChild = current->child + i;
            if( json->queued[currentChild - graph.root.ptr] <= 0) continue;

            //create queue node
            _queue = (QUEUE *)malloc(sizeof(QUEUE));
            _queue->node = currentChild;
            _queue->next = NULL;

            //append queue node
            if(queueTail == NULL){ 
                queueHead = queueTail = _queue;
            } else {
                queueTail->next = _queue;
                queueTail = queueTail->next;
            }
            _queue = NULL;

            //mark node by assigning id 
            json->queued[currentChild - graph.root.ptr] *= -1;

        }

        
        //pop QUEUE, set to current
        if(queueHead == NULL)break;        
        current = queueHead->node;
        if(queueHead == queueTail){
            free(queueHead);
            queueHead = queueTail = NULL;
        } else {
            _queue = queueHead;
            queueHead = queueHead->next;
            free(_queue);
            _queue = NULL;
        }

    } 

    fprintf(json->outFile,"        ]\n");
}
