#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void a_close(){printf("a closed\n");}
void a_init(){atexit(a_close);}
void b_close(){printf("b closed\n");}
void b_init(){atexit(b_close);}

struct aaa{
    int a[16];
};

int main(){
    struct aaa h[2];
    // printf("%X\n",&h[0]);
    // printf("%X\n",&h[1]);
    // printf("%X\n",&h[1] - &h[0]);
    
    
    int hh[2] = {12,19};
    // printf("%d\n",sizeof(int));
    // printf("%X\n",&hh[0]);
    // printf("%X\n",&hh[1]);
    // printf("%d\n",&hh[1] - &hh[0]);
    
    // printf("%d\n",hh[&h[1] - &h[0]]);
    printf("%d\n",(char *)&h[1] - (char *)&h[0]);

    return 0;
}