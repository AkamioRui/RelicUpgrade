#include<stdlib.h>
#include<stdio.h>
#include<string.h>

/* note
    memset(destinationString, 'character2fill', howManyCharacter);
*/

//get the binary representation (with "strlen" many character) of a number 
//by default it's not escaped, if AUTOLENGTH then escaped
#define AUTO_LENGTH 0
char *getbin(int number, char *string, int strlen ){
    if(strlen == AUTO_LENGTH){
        
        string[0] = '0';
        
        int ptr = 0;
        while(number){//not 0
            string[ptr++] = '0' + ('1' - '0')*(number%2);
            number >>= 1;
        }
        ptr = (ptr-1)*(ptr > 0); //if ptr is 0 then the last number is at 0, while if the ptr is not 0 then the last number will be at ptr - 1
        
        char temp;
        for(int start = 0, end = ptr; start<end; ){
            temp = string[start];
            string[start] = string[end];
            string[end] = temp;
            start++;
            end--;
        }
        
        string[ptr+1] = '\0';
        return string;
    } else {
        memset(string,'0',strlen);
        int ptr = strlen-1;
        while(number > 0){
            string[ptr--] = '0' + ('1' - '0')*(number%2);
            number = number >> 1;
        }
        return string;
    }
    
}
#undef AUTO_LENGTH

void printTreeVert(int totalLayer){
    int *state = (int *)calloc(totalLayer -1, sizeof(int));
    int totalNode = (1<<totalLayer)-1;
    
    int idx, depth, indent, wordlen = totalLayer,totalLineLayer;
    char bin[totalLayer+1];
    for(int i = 0 ; i<totalNode; i++ ){

        //find depth
        idx = i;
        depth = totalLayer - 1;
        while(idx%2 == 1){
            depth--;
            idx = idx >> 1;
        } 
        idx = idx >> 1;
        
        //print node dent
        indent = totalLayer-1-depth;//find how deep the indentation
        indent = wordlen*indent + (indent>3? 6+(indent-3)*3: indent*(indent+1)/2);//actual indent length
        printf("%*d\b", indent+1,1 );

        //print node 
        printf("%s", getbin(i,bin,totalLayer) );
        indent += wordlen;

        
        

        
        /* //0th layer
        if(idx%2 == 0) printf("\\");
        else printf("/");
        */

        //print connecterLine
        int lineIndent, totalLineLayer;
        totalLineLayer = totalLayer - 1;
        #define moveTo(layer) {\
            lineIndent = wordlen*((layer)+1) + ((layer)<4? (layer)*((layer)+1)/2: 6+((layer)-3)*3);\
            printf("%*.d\b", lineIndent-indent+1,1 );\
            indent = lineIndent;\
        }

        //for(int l = 0; l<totalLineLayer; l++){
        for(int l = 0; l<1; l++){
            //swtich based on phase: 0,1,2,3?
            switch ((i>>l) % 4){//at l, phase length is (i>>l)
                case 0:
                    //[   ]
                    //[   ]
                    //[   ][  ]
                    //[\  ][\ ][\] 
                    if(i%(1<<l) == (1<<l)-1){//print[\]
                        moveTo(l);
                        printf("\\%*.d\b", lineIndent,1, (l>2?2:l)+1,1 );
                        indent += (l<=2?l+1:3);
                        
                    } 
                break;

                case 1:
                    //[ | ]
                    //[ | ]
                    //[  \][ \]
                    //[   ][  ][ ]
                    if(i%(1<<l) == (1<<l)-1){//[]
                        moveTo(l);
                        printf("\\%*.d\b", lineIndent,1, (l>2?2:l)+1,1 );
                        indent += (l<=2?l+1:3);
                    } else if(i%(1<<l) == (1<<l)-2){//[ \]
                        moveTo(l);
                        printf("%*.d\b\\", lineIndent,1, (l>2?2:l)+1,1 );
                        indent += (l<=2?l+1:3);   
                    } else {//[ | ]
                        moveTo(l);
                        printf(" | ", lineIndent,1, (l>2?2:l)+1,1 );
                        indent += 3;
                    }
                break;
                
                case 2:
                    //[  /][ /]
                    //[ | ]
                    //[ | ]
                    //[/  ][/ ][/]
                    if(i%(1<<l) == (1<<l)-1){//[/  ]
                        moveTo(l);
                        printf("/%*.d\b", lineIndent,1, (l>2?2:l)+1,1 );
                        indent += (l<=2?l+1:3);
                    } else if(i%(1<<l) == 0){//[  /]
                        moveTo(l);
                        printf("%*.d\b/", lineIndent,1, (l>2?2:l)+1,1 );
                        indent += (l<=2?l+1:3);
                        
                    } else {//[ | ]
                        moveTo(l);
                        printf("%*.d\b/", lineIndent,1, (l>2?2:l)+1,1 );
                        indent += (l<=2?l+1:3);
                    
                    }
                break;

                
                case 3:
                //[   ]
                //[   ]
                //[   ][  ]
                //[   ][  ][ ]
                break;

                
    
            } 

           

        }

        //endl
        printf("\n");

    }

}

int main(){
    // printTreeVert(5);
    
    char buff[100];
    for(int i = 0 ; i<8; i++){
        getbin(i,buff, 0);
        printf("%d = %s\n",i,buff);
    }
    
    //printf("\\\n%c\n%c\n/\n",(char)179,(char)186);


    return 0;
}
/* 
[════]
      [════]
[════]
            [════]
[════]
      [════]
[════]
                  216.00
[════]
      [════]
[════]
            184.00
[════]
      152.80
121.60
                        184.48
[════]
      [════]
[════]
            184.00
[════]
      152.80
121.60
                  168.60
[════]
      152.80
121.60
            152.80
121.60
      137.20
121.60
done
  /
 /
 |
 |
/

*/