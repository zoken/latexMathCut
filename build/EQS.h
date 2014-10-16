class EQS{
    public:
    EQS(){beiyong_opname='\0';content=NULL;}
    char* content ;
    int op ;
    char opname ;
    char beiyong_opname ;
    EQS *nexteq ;
};
