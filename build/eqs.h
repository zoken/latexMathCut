#include <list>
#include <stdlib.h>

class EQS{
    public:
        EQS(){beiyong_opname='\0';content=NULL;}
        ~EQS(){
            if(content!=NULL){
                free(content);
                content = NULL ;
            }
        }
        char* content ;
        int op ;
        char opname ;
        char beiyong_opname ;
        EQS *nexteq ;
        static void combineEquations(EQS* head,std::list<char*>* resultlist);
};
