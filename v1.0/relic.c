#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//creation, +3,+,+ +, +, Win
#define costCreate 36.f
const double costUp[] = { 1, 2, 4, 7.8f, 15.6f};
#define costWin 121.6f


typedef struct Node{
    double winChance;
    double costAverage;
    double *nextChance;
}Node ;
typedef struct FlipNode{
    double winChance;
    double costAverage;
} FlipNode;


//all node
Node all;//next is {I3, I4}
#define PI3 0
#define PI4 1
Node I3;//next is {I3xyz} 
Node *I3xyz;//next = {II3_x}
Node *II3_x;//next = FlipNode4
FlipNode *flipnode4;
#define flip4Len 31
Node I4;//next is {I4_x}
Node *I4_x;//next = FlipNode5
FlipNode *flipnode5;
#define flip5Len 63




void printTreeHorizontal( FlipNode* tree, int treeLen){
    //find total layer
    //1 1
    //2 10 11
    //3 100 101 110 111
    int layer = 0;
    int _treeLen = treeLen;
    while(_treeLen > 0){
        layer++;
        _treeLen = _treeLen >> 1;
    }
    
    //length of initial space = len of final layer / 2
    int pad = 1;
    int wordLen = 6;
    int bufflen = (1 << layer-1) * (pad + wordLen + pad)/2; 
    char *buff = (char *)malloc(bufflen*sizeof(char));
    if(buff==NULL)perror("malloc ERROR\n");
    memset(buff,' ',bufflen*sizeof(char));
    buff[bufflen - wordLen/2] = '\0';
     
    //for some reason, I cant malloc more than 12 memory;

    //printing
    #define l(i) (2*(i) + 1)
    #define r(i) (2*(i) + 2)
    #define h(i) (((i)-1)/2 )
    
    //the rest
    char top[] = {(char)0xC9,(char)0xCD,(char)0xCD,(char)0xCD,(char)0xCD,(char)0xBB,'\0'};
    char mid[] = {(char)0xBA,(char)32,(char)32,(char)32,(char)32,(char)0xBA,'\0'};
    char bot[] = {(char)0xC8,(char)0xCD,(char)0xCD,(char)0xCD,(char)0xCD,(char)0xBC,'\0'};
    for(int start = 0,end = 0; start < treeLen;  ){
        int kk =0;
        //per layer: []a[][]b[][] 
        for(int i = start; i<=end; i++){
            if(tree[i].winChance > 0)printf("%s%6.2lf%s",buff,tree[i].costAverage/tree[i].winChance,buff);
            else printf("%s%s%s",buff,top,buff);  
        }
        printf("\n");
        for(int i = start; i<=end; i++){
            if(tree[i].winChance > 0)printf("%s%6.4lf%s",buff,tree[i].winChance,buff);
            else printf("%s%s%s",buff,mid,buff);
        }
        printf("\n");
        for(int i = start; i<=end; i++){
            if(tree[i].winChance > 0)printf("%s%6.1lf%s",buff,tree[i].costAverage,buff);
            else printf("%s%s%s",buff,bot,buff);
        }
        printf("\n");

        //preparing for next 
        start = l(start);
        end = r(end) < treeLen ? r(end) : treeLen -1;
        bufflen = bufflen/2;
        if(bufflen >= wordLen/2)buff[bufflen - wordLen/2] = '\0';
    }
    
    //deallocating
    free(buff);
    #undef l
    #undef r
    #undef h
}

#define minimized
void printTreeVertical( FlipNode* tree, int treeLen){
    //find total layer
    //1 1
    //2 10 11
    //3 100 101 110 111
    int totalLayer = 0;
    int _treeLen = treeLen;
    while(_treeLen > 0){
        totalLayer++;
        _treeLen = _treeLen >> 1;
    }

    int wordLen = 6;
    
    //printing
    int idxLayer, idx, _code;

    //char top[] = {(char)0xC9,(char)0xCD,(char)0xCD,(char)0xCD,(char)0xCD,(char)0xBB,'\0'};
    char *top = (char *)calloc(wordLen + 1, sizeof(char));
    memset(top,(char)0xCD,wordLen + 1);
    top[0] = (char)0xC9; top[wordLen-1] = (char)0xBB;top[wordLen] ='\0';

    //char mid[] = {(char)0xBA,(char)32,(char)32,(char)32,(char)32,(char)0xBA,'\0'};
    char *mid = (char *)calloc(wordLen + 1, sizeof(char));
    memset(mid,(char)32,wordLen + 1);
    mid[0] = (char)0xBA; mid[wordLen-1] = (char)0xBA;mid[wordLen] ='\0';

    //char bot[] = {(char)0xC8,(char)0xCD,(char)0xCD,(char)0xCD,(char)0xCD,(char)0xBC,'\0'};
    char *bot = (char *)calloc(wordLen + 1, sizeof(char));
    memset(bot,(char)0xCD,wordLen + 1);
    bot[0] = (char)0xC8; bot[wordLen-1] = (char)0xBC;bot[wordLen] ='\0';

    char *not = (char *)calloc(wordLen + 1, sizeof(char));
    memset(not,(char)0xCD,wordLen + 1);
    not[0] = '['; not[wordLen-1] = ']';not[wordLen] ='\0';

 
    for(int code = 0; code< (1<<totalLayer) - 1; code++ ){
        //find layer from code
        _code = code;
        idxLayer = totalLayer-1;
        while(_code%2 == 1){
            idxLayer--;
            _code = _code >>1;
        }
        _code = _code>>1;
        //idx = layer + code
        idx = (1<<idxLayer) - 1 + _code;

        //printing
        #ifdef minimized
        if(tree[idx].winChance > 0){
            printf("%*d\b%*.2lf\n", wordLen*(totalLayer-1-idxLayer)+1,1, wordLen,tree[idx].costAverage/tree[idx].winChance );
        } else {
            printf("%*d\b%s\n", wordLen*(totalLayer - 1 - idxLayer)+1,1, not);
        }
        #endif

        #ifndef minimized
        if(tree[idx].winChance > 0){
            printf("%*d\b%*.2lf\n", wordLen*(totalLayer-1-idxLayer)+1,1, wordLen,tree[idx].costAverage/tree[idx].winChance );
            printf("%*d\b%*.*lf\n", wordLen*(totalLayer-1-idxLayer)+1,1, wordLen,wordLen-2,tree[idx].winChance);
            printf("%*d\b%*.2lf\n", wordLen*(totalLayer-1-idxLayer)+1,1, wordLen,tree[idx].costAverage);
        } else {
            printf("%*d\b%s\n", wordLen*(totalLayer - 1 - idxLayer)+1,1, top);
            printf("%*d\b%s\n", wordLen*(totalLayer - 1 - idxLayer)+1,1, mid);
            printf("%*d\b%s\n", wordLen*(totalLayer - 1 - idxLayer)+1,1, bot);
        }
        #endif
        

    }



    free(top);
    free(mid);
    free(bot);
    free(not);
    #undef minimized
}


#define flip4 4
#define flip5 5

FlipNode* createFlip(int flipType, int minimalUpgrade, int numberOfGoodSub){
    //create tree
    #define l(i) (2*(i) + 1)
    #define r(i) (2*(i) + 2)
    #define h(i) (((i)-1)/2 )
    int deepestLayer = flipType;
    int treeLen = (2 << deepestLayer) -1;// 2 * 2^deepestLayer - 1
    FlipNode* tree = (FlipNode *)calloc(treeLen,sizeof(FlipNode));
    

    //adding assign upgrade
    int* treeUpgrade = (int *)calloc(treeLen,sizeof(int));
    treeUpgrade[0] = 0;
    for(int ptr = 0; ptr<=h(treeLen-1); ptr++ ){
        treeUpgrade[l(ptr)] = treeUpgrade[ptr];
        treeUpgrade[r(ptr)] = treeUpgrade[ptr] + 1;  
    }

    //assign win and cost at the last layer
    for(int ptr = (1<<deepestLayer) - 1; ptr < treeLen ; ptr++){
        if(treeUpgrade[ptr] >= minimalUpgrade){
            tree[ptr].winChance = 1;
            tree[ptr].costAverage = costWin;
        } else {
            tree[ptr].winChance = 0;
            tree[ptr].costAverage = 0;
        }
        
    }

    //assign win and cost from bottom to top
    double pwin = numberOfGoodSub/4.f;
    double plose = 1 - pwin;
    int costDepth = 5;
    for(int start = (1<<deepestLayer) - 1, end =  treeLen - 1; start > 0; ){//initialized at the lowest layer
        start = h(start);//move to layer above it
        end = h(end);
        costDepth--;//start at layer 4, cost 

        for(int ptr = start; ptr <= end; ptr++){//a layer
            //if either left or right have a chance to win, take into accout
            if(tree[l(ptr)].winChance >0 || tree[r(ptr)].winChance > 0){
                tree[ptr].winChance   = plose*tree[l(ptr)].winChance   + pwin*tree[r(ptr)].winChance;
                tree[ptr].costAverage = plose*tree[l(ptr)].costAverage + pwin*tree[r(ptr)].costAverage + costUp[costDepth];
                
            } else {
                tree[ptr].winChance   = 0;
                tree[ptr].costAverage = 0;

            }

            
        }

        
        
    }
     
    //printTreeHorizontal(tree, treeLen);

    free(treeUpgrade);
    
    return tree;

    #undef l
    #undef r
    #undef h
    
}

int main(){
    
    
    int numberOfGoodSub = 2;
    int minimalUpgrade = 3;
    FlipNode *tree = createFlip(flip4, minimalUpgrade, numberOfGoodSub);
    printTreeVertical(tree, flip4Len);
   
     
    /* for(int numberOfGoodSub = 2; numberOfGoodSub>=1; numberOfGoodSub-- ){
        for(int minimalUpgrade = 4; minimalUpgrade>=1; minimalUpgrade-- ){
            printf(" numberOfGoodSub = %d , minimalUpgrade = %d :\n", numberOfGoodSub , minimalUpgrade);
            showProbability(minimalUpgrade, numberOfGoodSub);
        }
    } */
    printf("done");
    return 0;
}