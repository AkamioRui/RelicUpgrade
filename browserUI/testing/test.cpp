#include<fstream>


int main(){
    std::ofstream file("mydata.json");
    file << "\
[\n\
    {\n\
        \"nodes\":[\n\
            {\"x\":1,\"y\":1},\n\
            {\"x\":2,\"y\":4},\n\
            {\"x\":3,\"y\":9},\n\
            {\"x\":4,\"y\":16},\n\
            {\"x\":5,\"y\":25},\n\
            {\"x\":6,\"y\":36},\n\
            {\"x\":7,\"y\":49}\n\
        ],\n\
        \"links\":[\n\
            {\"src\":0,\"dst\":1},\n\
            {\"src\":1,\"dst\":2},\n\
            {\"src\":2,\"dst\":3},\n\
            {\"src\":3,\"dst\":4},\n\
            {\"src\":4,\"dst\":5},\n\
            {\"src\":5,\"dst\":6},\n\
            {\"src\":6,\"dst\":0}\n\
\n\
\n\
        ]\n\
    },\n\
    {\n\
        \"nodes\":[],\n\
        \"links\":[\n\
            {\"src\":0,\"dst\":1},\n\
            {\"src\":0,\"dst\":2},\n\
            {\"src\":0,\"dst\":3},\n\
            {\"src\":1,\"dst\":4},\n\
            {\"src\":2,\"dst\":5},\n\
            {\"src\":3,\"dst\":6}\n\
        ]\n\
    }\n\
\n\
]\n\
    "; 
    
    
    file.close();
    
    

    return 0;
}