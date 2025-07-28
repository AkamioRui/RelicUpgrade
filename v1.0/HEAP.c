#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct HEAP{
    int data;
    struct HEAP* left;
    struct HEAP* right;
} HEAP;


// struct LIST{
//     HEAP *node;
//     int visiting;
//     struct LIST *nextList;
// };
int HEAP_findDepth(HEAP *root){
    //just definition
    #define HEAP_NULL -1
    #define HEAP_LEFT 0
    #define HEAP_RIGHT 1
    struct LIST{
        HEAP *node;
        int visiting;
        struct LIST *nextList;
    } *stack, *tempstack;
    // struct LIST *stack, *tempstack;
    
    

    
    if(root == NULL) return 0;
    //initialize
    int result = 0;
    stack = NULL;
    HEAP *current = root;
    int haveVisited = HEAP_NULL;
    int depth = 1;

    do{
        ///* test */ printf("%c = %llX\n",current->data,( long long unsigned int)current);
        switch(haveVisited){
            case HEAP_NULL:
            if(current->left){
                tempstack = (struct LIST*) malloc(sizeof(struct LIST));
                tempstack->node = current;
                tempstack->visiting = HEAP_LEFT;
                tempstack->nextList = stack;
                stack = tempstack;
                
                current = current->left;  
                haveVisited = HEAP_NULL;  
                depth++;
                break;
            }
    
            case HEAP_LEFT://then go visit right
            if(current->right){
                tempstack = (struct LIST*) malloc(sizeof(struct LIST));
                tempstack->node = current;
                tempstack->visiting = HEAP_RIGHT;
                tempstack->nextList = stack;
                stack = tempstack;
    
                current = current->right;        
                haveVisited = HEAP_NULL; 
                depth++;
                break;
            }
            case HEAP_RIGHT://get out
                
                result += (depth>result)*(depth - result);
                ///* test */printf("pop %c = %d\n",current->data,depth);
                if(stack){
                    current = stack->node;
                    haveVisited = stack->visiting;
                    depth--;
    
                    tempstack = stack;
                    stack = stack->nextList;
                    free(tempstack);
                }
                else current = NULL;
                break;
            
            
            default: printf("error haveVisited = %d\n",haveVisited); break;
        } 
    } while(current);

    return result;
    #undef HEAP_LEFT
    #undef HEAP_RIGHT
    #undef HEAP_NULL
}
void HEAP_print(HEAP *root){

    int totaldepth = HEAP_findDepth(root);

    //DFS inorder
    #define NONE 0
    #define LEFT 1
    #define RIGHT 2
    struct LIST{
        void *node;
        int visiting;
        struct LIST *next;
    } *stack = NULL, *tempstack;
    HEAP* current = root;
    int hasVisited = NONE;
    int depth = 1;

    //for print style
        
        
    //
    while(current){
        switch(hasVisited){
            case NONE:
            if(current->left){
                tempstack = (struct LIST*)calloc(1,sizeof(struct LIST));
                tempstack->node = current;
                tempstack->visiting = LEFT;
                tempstack->next = stack;
                stack = tempstack;

                current = current->left;
                hasVisited = NONE;
                depth++;
                break;
            }
                
            case LEFT:
            /* print style *///-->ddd/ddd//ddd/|/ddd/|/...
                int nodePrintLength = 2;
                int layer = totaldepth - depth;//start from 0
                printf("%*d",(layer+1)*nodePrintLength+(layer<4? layer*(layer+1)/2: 6+(layer-3)*3),current->data);
                
                
                int previousDirection = NONE;
                int currentLayer = layer+1;
                int flipCount = 0;
                for(struct LIST *stacknode = stack; stacknode != NULL; stacknode = stacknode->next){
                    int layerWidth = nodePrintLength+(currentLayer>3?3:currentLayer);
                    /* 
                    basicly
                    1. it will only print non space if the parent change direction(from left to right vice versa)
                    2.  / or \ only appear when adjacent to a node (if flatten), only layer 0 can have it, 
                    and (conjecture) only at the second time
                    3. the rest of parent changing direction is just |
                    
                    */
                    
                    if(previousDirection == stacknode->visiting) {
                        printf("%*d\b ",layerWidth,1);
                    } else {
                        switch (flipCount){
                            case 0:
                                printf("%c%*d\b ", (stacknode->visiting == RIGHT?'/':'\\'), layerWidth-1, 1);
                                break;
                            case 1:
                                if(layer == 0) {
                                    if(currentLayer <=2) 
                                    printf(" %c%*d\b ",(stacknode->visiting == RIGHT?'/':'\\'), layerWidth-2, 1);
                                    else 
                                    printf("  %c%*d\b ",(stacknode->visiting == RIGHT?'/':'\\'), layerWidth-3, 1);
                                    break;
                                } 
                            default:
                                    printf(" |%*d\b ", layerWidth-2, 1);
                                break;
                            
                        }
                        

                        previousDirection = stacknode->visiting;
                        flipCount++;
                    }

                    currentLayer++;
                }
                printf("\n");

            /* print style */


            if(current->right){
                tempstack = (struct LIST*)calloc(1,sizeof(struct LIST));
                tempstack->node = current;
                tempstack->visiting = RIGHT;
                tempstack->next = stack;
                stack = tempstack;

                current = current->right;
                hasVisited = NONE;
                depth++;
                break;        
            }

            
            case RIGHT:
                if(stack){
                    current = stack->node;
                    hasVisited = stack->visiting;
                    depth--;

                    tempstack = stack;
                    stack = stack->next;
                    free(tempstack);
                } else current = NULL;
                
            break;
            default: printf("error HEAP_print DFS hasVisited = %d\n",hasVisited); break;
        }
        
    }
    
    #undef NONE 
    #undef LEFT 
    #undef RIGHT



}




int main(){
    int len = (1<<5)-1;
    HEAP *a = (HEAP*)calloc(len,sizeof(HEAP));
    for(int i = 0; i<len; i++){
        a[i].data = i;
    }
    for(int i = 0; i<((len+1)>>1)-1; i++){
        a[i].left = a+2*i+1;
        a[i].right = a+2*i+2;
    }


    HEAP_print(a);

    free(a);


    return 0;
}
/*
\ 
 |
 \


    HEAP *a,*b,*c,*d,*e,*f,*g;
    a=calloc(sizeof(HEAP),1);
    b=calloc(sizeof(HEAP),1);
    c=calloc(sizeof(HEAP),1);
    d=calloc(sizeof(HEAP),1);
    e=calloc(sizeof(HEAP),1);
    f=calloc(sizeof(HEAP),1);
    g=calloc(sizeof(HEAP),1);
    
    a->data = 'a';
    b->data = 'b';
    c->data = 'c';
    d->data = 'd';
    e->data = 'e';
    f->data = 'f';
    g->data = 'g';
    
    a->left = b;
    a->right = c;
    b->right = f;
    f->right = g;
    c->right = d;
    c->left = e;

    // printf("%c,left = %llu,right = %llu\n",a->data,( long long unsigned int)a->left,( long long unsigned int)a->right);
    // printf("%c,left = %llu,right = %llu\n",b->data,( long long unsigned int)b->left,( long long unsigned int)b->right);
    // printf("%c,left = %llu,right = %llu\n",c->data,( long long unsigned int)c->left,( long long unsigned int)c->right);
    // printf("%c,left = %llu,right = %llu\n",d->data,( long long unsigned int)d->left,( long long unsigned int)d->right);
    // printf("%c,left = %llu,right = %llu\n",e->data,( long long unsigned int)e->left,( long long unsigned int)e->right);
    // printf("%c,left = %llu,right = %llu\n",f->data,( long long unsigned int)f->left,( long long unsigned int)f->right);
    // printf("%c,left = %llu,right = %llu\n",g->data,( long long unsigned int)g->left,( long long unsigned int)g->right);


*/