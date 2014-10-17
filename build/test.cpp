#include "mathwordtools.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>

int main(int argc, char* argv[]){
    MathWordTool mwt ;
    list<char*> resultlist ;
    mwt.cut(argv[1],&resultlist);
    list<char*>::iterator iter ;
    for(iter=resultlist.begin();iter!=resultlist.end();iter++){
        cout << *iter << endl;
    }
}
