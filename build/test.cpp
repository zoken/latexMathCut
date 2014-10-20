#include "mathwordtools.h"
#include <iostream>

int main(int argc, char* argv[]){
    MathWordTool mwt ;
    std::list<char*> resultlist ;
    mwt.cut(argv[1],&resultlist);
    std::list<char*>::iterator iter ;
    for(iter=resultlist.begin();iter!=resultlist.end();iter++){
        std::cout << *iter << std::endl;
    }
}
