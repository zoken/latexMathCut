#include "eqs.h"
#include <string.h>
/*************************************************************
    method：combineEquations
    用途：合并公式表达式
    输入EQS列表，输出可以结合的数学表达式，并存入切词结果results
    内存处理：释放整个公式链表，释放所有非空EQS.content
****************a*********************************************/
void EQS::combineEquations(EQS* head,std::list<char*>* resultlist){
    int currentMaxOp = 0 ;
    EQS *maxEquations,*current,*tofree_eq ;
    char* tmp ;
    if(head == NULL)
        return;
    while(head->nexteq!=NULL){
        current = head ;
        maxEquations = head ;
        currentMaxOp = 0;
        while(current->nexteq!=NULL){
            if(current->op == 0)
                break ;
            if(current->op > currentMaxOp){
                currentMaxOp = current->op ;
                maxEquations = current ;
                }
            current = current->nexteq ;
        }
        if(head == current){
            head = current->nexteq ;
            delete current;
            continue ;
        }
        current = maxEquations;
        if(current->content==NULL||current->nexteq->content==NULL){
            head = current->nexteq; //error operate.
            free(current);
            continue ;
        }
        int currentLen = strlen(current->content);
        int nextLen = strlen(current->nexteq->content);
        if(current->beiyong_opname=='\0'){
            tmp = (char*)malloc(sizeof(char)*(currentLen+nextLen+2));
            strcpy(tmp,current->content);
            tmp[currentLen] = current->opname ;
            strcpy(tmp+currentLen+1,current->nexteq->content);
        }else{
            tmp = (char*)malloc(sizeof(char)*(currentLen+nextLen+3));
            strcpy(tmp,current->content);
            tmp[currentLen] = current->opname ;
            tmp[currentLen+1] = current->beiyong_opname;
            strcpy(tmp+currentLen+2,current->nexteq->content);
        }
        current->content = tmp ;
        tmp = (char*)malloc(sizeof(char)*(strlen(tmp)+1));
        strcpy(tmp,current->content);
        current->op = current->nexteq->op ;
        current->beiyong_opname = current->nexteq->beiyong_opname;
        current->opname = current->nexteq->opname ;
        tofree_eq = current->nexteq ;
        current->nexteq = current->nexteq->nexteq;
        delete tofree_eq;
        resultlist->push_back(tmp);//这个tmp是不能删除的，所以重新创建了
    }
}
