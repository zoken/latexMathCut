#include <stdlib.h>
#include "latexparser.h"
#include "equation.h"
#include "mathwordtools.h"

using namespace std ;

int main(int argc,char *argv[]){
	MathWordTool mwt ; 
    list<char*> result;
    printf("input:%s\n",argv[1]);
	mwt.cut(argv[1],&result);
    list<char*>::iterator iter;
    for(iter=result.begin();iter!=result.end();iter++){
        printf("-------result--------\n");
        printf("%s\n",*iter);
    }
    return 0;
}
