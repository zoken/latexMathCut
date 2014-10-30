#include "mathwordtools.h"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]){
    char buf[4096];   
    MathWordTool mwt ;
    std::list<char*> resultlist ;
    std::ifstream testfile(argv[1]);
    std::list<char*>::iterator iter ;
    if(!testfile.is_open()){
        std::cout << "error opening file" << std::endl ;
        exit(1);
    }else{
        while(!testfile.eof()){
            testfile.getline(buf,4096);
            resultlist.clear();
            mwt.cut(buf,&resultlist);
            std::cout << "----------------------------" << std::endl;
            for(iter=resultlist.begin();iter!=resultlist.end();iter++){
                std::cout << *iter << std::endl;
            }
        }
        testfile.close();
    }

  //  mwt.cut(argv[1],&resultlist);
    /**
    std::list<char*>::iterator iter ;
    for(iter=resultlist.begin();iter!=resultlist.end();iter++){
        std::cout << *iter << std::endl;
    }
    */
}
