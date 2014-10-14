#include <stdlib.h>
#include "latexparser.h"
#include "equation.h"
#include "mathwordtools.h"
#include "time.h"
using namespace std ;

int main(int argc,char *argv[]){
	MathWordTool mwt ; 
    list<char*> result;
    int executeNum = 10000;
    int i = 0 ;
//    for(i =0 ; i < executeNum ; i++){
    	mwt.cut(argv[1],&result);
//    }
    //output
    list<char*>::iterator iter;
    for(iter=result.begin();iter!=result.end();iter++){
        printf("%s\n",*iter);
    }
    return 0;
}
