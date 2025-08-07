#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//note
#pragma region
//destructor atexit
    void a_close(){printf("a closed\n");}
    void a_init(){atexit(a_close);}
    void b_close(){printf("b closed\n");}
    void b_init(){atexit(b_close);}


struct aaa{
    int a[16];
};
#pragma endregion

void printGraphPrinting(){
    FILE *inFile = fopen("mydata.json","r");
    FILE *outFile = fopen("111Test","w");
    FILE *mytmpFile = tmpfile();

    //writting to temp
    int count;
    for(char buffer[1024]; (count = fread(buffer,1,1024,inFile)) != 0; ){
        fwrite(buffer,1,count,mytmpFile);
    }
    rewind(mytmpFile);//try not

    for(char buffer[1024]; (count = fread(buffer,1,1024,mytmpFile)) != 0;){
        fwrite(buffer,1,count,outFile);
    }
    
    fclose(mytmpFile);
    fclose(outFile);
    fclose(inFile);

}


void fgetsOverflowTest(FILE *inFile,FILE *outFile){
    for(char buff[3], code; ;){
        code = fgets(buff,3,inFile);
        if(code == NULL) break;
        fputs(buff,outFile);
        fputc(',',outFile);
    }
}
void printAll(FILE *inBinFile, FILE *outFile){
    fseek(inBinFile,0,SEEK_END);
    int length = ftell(inBinFile);
    rewind(inBinFile);
    /*  */printf("%d\n",length);
    

    char *data = malloc(length+1);
    data[length] = '\0';
    fread(data,sizeof(char),length,inBinFile);
    /*  */for(int i = 0; i<length; i++){printf(" [%d]",data[i]);}//need {}?
    /*  */printf("[%s]\n",data);

    fprintf(outFile,"%s",data);
    free(data);
}
void printAll2(FILE *inFile, FILE *outFile){
    char c;
    while(1){
        c = fgetc(inFile);
        if(!feof(inFile))fputc(c,outFile);
        else break;
    }
}
void fseek2delete(){
    FILE *outFile = fopen("111Test.json","w+");
    
    
    fprintf(outFile,"[\n");
    
    fprintf(outFile,"[1],\n");
    fprintf(outFile,"[1],\n");

    // fseek(outFile,-3,SEEK_CUR);
    // fprintf(outFile," \n");//fine up til here

    // rewind(outFile);
    // for(char buff; (buff = fgetc(outFile)) != EOF ; printf("[%d] ",buff)){}
    // printf("\n");

    fseek(outFile,4,SEEK_SET);
    fprintf(outFile,"2");

    // fflush(outFile);
    // rewind(outFile);
    // for(char buff; (buff = fgetc(outFile)) != EOF; printf("[%d] ",buff)){}
    // printf("\n");

    fseek(outFile,0,SEEK_END);
    fprintf(outFile,"]\n");


    fclose(outFile);
}
void print2(){
    printf("2\n");
}
#define say_generic(MSG)\
void say_##MSG(){\
    printf(#MSG);\
}
say_generic(hello)
int main(){

    say_hello();
    return 0;
}