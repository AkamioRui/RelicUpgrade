#include<iostream>
using namespace std;

int main(){
    char c1 = 0xC9;
    char c2 = 0xBB;
    char c3 = 0xBC;
    char c4 = 0xC8;
    char ch = 0xCD;
    char cv = 0xBA;
    cout <<"0xC9 = " << (char)0xC9 << endl;  
    cout <<"0xBB = " << (char)0xBB << endl;  
    cout <<"0xBC = " << (char)0xBC << endl;  
    cout <<"0xC8 = " << (char)0xC8 << endl;  
    cout <<"0xCD = " << (char)0xCD << endl;  
    cout <<"0xBA = " << (char)0xBA << endl;  

    char ss = ' ';
    cout<<c1<<ch<<ch<<ch<<ch<<c2<<endl;
    cout<<cv<<ss<<ss<<ss<<ss<<cv<<endl;
    cout<<c4<<ch<<ch<<ch<<ch<<c3<<endl;
    char top[] = {(char)0xC9,(char)0xCD,(char)0xCD,(char)0xCD,(char)0xCD,(char)0xBB,'\0'};
    char mid[] = {(char)0xBA,(char)32,(char)32,(char)32,(char)32,(char)0xBA,'\0'};
    char bot[] = {(char)0xC8,(char)0xCD,(char)0xCD,(char)0xCD,(char)0xCD,(char)0xBC,'\0'};
    cout<<top<<endl;
    cout<<mid<<endl;
    cout<<bot<<endl;

    

    return 0;

}

